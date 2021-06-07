def add_sounds(sound1, sound2):
    new_sound = {}

    new_sound['rate'] = sound2['rate']
    new_sound['left'] = []
    new_sound['right'] = []

    for _ in range(3):
        for i in range(len(sound1['left'])):
            new_sound['left'].append(sound1['left'][i])
            new_sound['right'].append(sound1['right'][i])
        
        for __ in range(20000):
            new_sound['left'].append(0)
            new_sound['right'].append(0)

    
    for i in range(len(sound2['left'])):
        if len(new_sound['right']) < 2900000:
            new_sound['left'].append(sound2['left'][i])
            new_sound['right'].append(sound2['right'][i])

    return new_sound

import io
import wave
import struct

def load_wav(filename):
    """
    Given the filename of a WAV file, load the data from that file and return a
    Python dictionary representing that sound
    """
    f = wave.open(filename, 'r')
    chan, bd, sr, count, _, _ = f.getparams()

    assert bd == 2, "only 16-bit WAV files are supported"

    left = []
    right = []
    for _ in range(count):
        frame = f.readframes(1)
        if chan == 2:
            left.append(struct.unpack('<h', frame[:2])[0])
            right.append(struct.unpack('<h', frame[2:])[0])
        else:
            datum = struct.unpack('<h', frame)[0]
            left.append(datum)
            right.append(datum)

    left = [i / (2 ** 15) for i in left]
    right = [i / (2 ** 15) for i in right]

    return {'rate': sr, 'left': left, 'right': right}

def write_wav(sound, filename):
    """
    Given a dictionary representing a sound, and a filename, convert the given
    sound into WAV format and save it as a file with the given filename (which
    can then be opened by most audio players)
    """
    outfile = wave.open(filename, 'w')
    outfile.setparams((2, 2, sound['rate'], 0, 'NONE', 'not compressed'))

    out = []
    for l, r in zip(sound['left'], sound['right']):
        l = int(max(-1, min(1, l)) * (2 ** 15 - 1))
        r = int(max(-1, min(1, r)) * (2 ** 15 - 1))
        out.append(l)
        out.append(r)

    outfile.writeframes(b''.join(struct.pack('<h', frame) for frame in out))
    outfile.close()

def make_sound_file(soundfile):
    beep_sound = load_wav('songs/beep.wav')
    this_sound = load_wav(soundfile)

    mix = add_sounds(beep_sound, this_sound)

    new_name = soundfile[0: len(soundfile) - 4]
    write_wav(mix, new_name + "_beep.wav")

def song_to_beats_file(soundfile, songname, bpm):
    note_file = open(songname + ".txt", "a")
    note_max = max(max(soundfile['left']),max(soundfile['right']))
    note_min = min(min(soundfile['left']), min(soundfile['right']))
    note_file.write(str(bpm) + "\n\n")

    for i in range(0, len(soundfile['left']), 15000):
        if i+14999 > len(soundfile['left']):
            list_max = len(soundfile['left'])
        else:
            list_max = i+14999

        left_list = [soundfile['left'][j] for j in range(i, list_max)]
        right_list = [soundfile['right'][k] for k in range(i, list_max)]
        left_avg = sum(left_list)/len(left_list)
        right_avg = sum(right_list)/len(right_list)

        if left_avg > 0:
            note_file.write("0" + "\t0.5\n")
        else:
            note_file.write("1" + "\t0.5\n")

        if right_avg > 0:
            note_file.write("3" + "\t0.5\n")
        else:
            note_file.write("2" + "\t0.5\n")

    note_file.close()

if __name__ == '__main__':
    # pass 
    #print(load_wav('songs/rocketman.wav')['rate'])
    #print(len(load_wav('songs/techno.wav')['left']))

    make_sound_file('songs/sunflower.wav')
    # song_to_beats_file(load_wav('songs/techno.wav'), "techno", 126)
