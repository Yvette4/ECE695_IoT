import os
import csv

def generate_csv_files(dir_path=None, file_path=None):
    files = []
    if dir_path:
        for entry in os.listdir(dir_path):
            if entry.endswith('.txt'):
                files.append(dir_path + '/' + entry)
    if file_path:
        files.append(file_path)
    if not files:
        print('No files or directory listed')
        return
    for entry in files:
        data = {}
        with open(entry, 'r') as fh:
            for line in fh:
                if 'started data collection' in line:
                    continue
                split_txt = line.strip().replace(',', '').split(' ')
                time = split_txt[1]
                if time not in data:
                    data[time] = split_txt[-3:]
                else:
                    data[time].extend(split_txt[-3:])
        # write data to new csv file
        csv_filename = os.path.splitext(entry)[0] + '.csv'
        with open(csv_filename, 'w', newline='') as fh:
            writer = csv.writer(fh)
            for _, value in data.items():
                # only store timestamps with both acc and gyro data
                if len(value) == 6:
                    writer.writerow(value)

# NOTE: run this file from this folder (don't run from the main ECE695_IoT folder)
# raw_file = './RawData/1_Typing.txt'
# generate_csv_files(file_path=raw_file)
raw_data_path = './RawData/'
generate_csv_files(dir_path=raw_data_path)
