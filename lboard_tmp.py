import sqlite3
import datetime

def request_handler(request):

    LIST_LENGTH = 8
    time = datetime.datetime.now()

    one_day_ago = time - datetime.timedelta(hours=24)

    db = "/var/jail/home/team43/fddr/lboard_song.db"
    conn = sqlite3.connect(db)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS lboard_song (user text, score real, time timestamp, song text);""")

    if 'POST' in request['method']:
        user = request['form']['user']
        score = request['form']['score']
        song = request['form']['song']
        c.execute('''INSERT into lboard_song VALUES (?,?,?,?);''', (user, score, time, song))
        
    data = c.execute("""SELECT * FROM lboard_song WHERE time > ?""", (one_day_ago,)).fetchall()
    conn.commit()
    conn.close()

    # process and return data accordingly...
    i = 0
    users = []
    scores = []
    for item in data:  # get all recent users and their scores
        users.append(item[0])
        scores.append(item[1])
        i += 1

    if LIST_LENGTH > len(data):
        LIST_LENGTH = data

    ret = []
    for num in range(len(data)):  # put them in order, only find top 8
        top_score = scores.index(max(scores))
        ret.append(scores.pop(top_score))
        ret.append(users.pop(top_score))

    return ret


