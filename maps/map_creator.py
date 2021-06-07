def convert_beats_to_millis(beats_per_minute, notes):
    """Converts the input's rest intervals from beats to milliseconds
    Used by make_beat_map, requires song's BPM"""
    millis_per_beat = 60000/beats_per_minute
    for note in notes:
        note[1] = note[1] * millis_per_beat


def make_beat_map(filename):
    """Given a proerly formatted txt file and a filename, creates a map that can be read by the ESP
    First line of txt must be the number value of the song's BPM
    Each successive line that isn't blank or commented will be interpreted as a note
    Short Note format: [note type, delay]
    Long Note format: [note type, delay, length]
    Note type: the unique note the player has to input
    Delay: the amount of time since the index before it until the note must be played
    Length: the amount of time the note must be played
    """
    with open(filename, encoding="utf-8") as f:
        text = f.read()
        # Breaks lines by \n
        tokens = text.split('\n')
        # Line 0 should always be the BPM
        bpm = float(tokens[0])
        notes = []
        for i in range(1, len(tokens)):
            # Ignores blank lines and lines that start with *
            if len(tokens[i]) == 0 or tokens[i][0] == '*':
                continue
            # Values are separated by tabs
            vals = tokens[i].split('\t')
            if len(vals) == 2:  # short notes
                timeVal = 0
                if '/' in vals[1]:
                    splitter = vals[1].split('/')
                    timeVal = float(splitter[0])/float(splitter[1])
                else:
                    timeVal = float(vals[1])
                notes.append([int(vals[0]), timeVal])
            else:  # Long notes
                timeVal1 = 0
                timeVal2 = 0
                if '/' in vals[1]:
                    splitter = vals[1].split('/')
                    timeVal = float(splitter[0])/float(splitter[1])
                else:
                    timeVal = float(vals[1])
                if '/' in vals[2]:
                    splitter = vals[2].split('/')
                    timeVal = float(splitter[0])/float(splitter[1])
                else:
                    timeVal = float(vals[2])
                notes.append([int(vals[0]), timeVal1, timeVal2])

        # Converts all beats to milliseconds, for the ESP's readability
        convert_beats_to_millis(bpm, notes)
        output = notes
        for i in range(len(notes)-1, -1,-1):
            total = 0
            for z in notes[:i+1]:
                total+=z[1]
            output[i][1] = round(total)
        return notes

# Runner for testing and debugging (local)
# if __name__ == '__main__':
#     print(make_beat_map('map.txt'))
