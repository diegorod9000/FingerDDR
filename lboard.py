import sqlite3
import datetime
import json

def request_handler(request):

    LIST_LENGTH = 8
    time = datetime.datetime.now()

    one_day_ago = time - datetime.timedelta(hours=24)

    db = "/var/jail/home/team43/fddr/lboard.db"
    conn = sqlite3.connect(db)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS lboard_final (user text, song text, score real, time timestamp);""")

    if 'POST' in request['method']:
        user = request['form']['user']
        song = request['form']['song']
        score = request['form']['score']
        c.execute('''INSERT into lboard_final VALUES (?,?,?,?);''', (user, song, score, time))
        data = c.execute("""SELECT * FROM lboard_final WHERE time > ? AND song = ?""", (one_day_ago, song)).fetchall()
    else:
        data = c.execute("""SELECT * FROM lboard_final WHERE time > ?""", (one_day_ago,)).fetchall()

    conn.commit()
    conn.close()

    # process and return data accordingly...
    i = 0
    users = []
    scores = []
    for item in data:  # get all recent users and their scores
        users.append(item[0])
        scores.append(item[2])
        i += 1

    if LIST_LENGTH > len(data):
        LIST_LENGTH = len(data)

    fin_users = []
    fin_scores = []
    while len(fin_users) < LIST_LENGTH and len(users) > 0:  # put them in order, only find top 8, only one of each username
        top_score_ind = scores.index(max(scores))
        top_user = users.pop(top_score_ind)
        top_score = scores.pop(top_score_ind)
        if top_user not in fin_users:
            fin_scores.append(top_score)
            fin_users.append(top_user)

    ret = {}
    ret['length'] = len(fin_users)
    ret['data'] = []
    for i in range(len(fin_users)):
        ret['data'].append(fin_users[i] + " " + str(int(fin_scores[i])))

    return json.dumps(ret)


