import os
import csv
import librosa
import soundfile as sf
from pydub import AudioSegment

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


def process_audio_files(dir_path):
    for entry in os.listdir(dir_path):
        if entry.endswith('.m4a'):
            # convert m4a file to wav and downsample to 22.05 kHz
            sr = 22050
            m4a_file = dir_path + entry
            wav_filename = os.path.splitext(entry)[0] + '.wav'
            track = AudioSegment.from_file(m4a_file,  format= 'm4a', frame_rate=sr)
            track.export(dir_path + wav_filename, format='wav')


# raw_file = './RawData/1_Typing.txt'
# generate_csv_files(file_path=raw_file)
raw_data_path = './Watch_App/RawData/'
generate_csv_files(dir_path=raw_data_path)
process_audio_files(raw_data_path)
