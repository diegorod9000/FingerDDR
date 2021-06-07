import sqlite3
import datetime
def request_handler(request):

    time_ = datetime.datetime.now()
    db = "/var/jail/home/team43/databases/current_song.db"
    conn = sqlite3.connect(db)
    c = conn.cursor()

    #c.execute(
    #    """CREATE TABLE IF NOT EXISTS current_song (song text, time timestamp);""")
    
    if "POST" in request['method']:
        tmp_song = request['form']['song']
        c.execute('''INSERT into current_song VALUES (?, ?);''', (tmp_song, time_))
        song = c.execute("""SELECT * FROM current_song ORDER BY time DESC""").fetchall()[0][0]
    else:
        song = c.execute("""SELECT * FROM current_song ORDER BY time DESC""").fetchall()[0][0]
    conn.commit()
    conn.close()

    return f'''<!DOCTYPE html>
    <html>
    <head>
        <link rel="stylesheet" href="game.css">
    </head>
    <body>
        <div>
        <h1>FINGER DANCE DANCE REVOLUTION</h1>
        <a href="http://608dev-2.net/sandbox/sc/team43/fddr/leaderboard.py" class="button">Go To Leaderboard</a>
        </div>
        
        <div class='audio-container'>
        <audio controls autoplay>
        <source src="songs/''' + str(song) + '''_beep.wav?''' + str(time_) + '''"type="audio/wav">
        Your browser does not support the audio element.
        </audio>
        </div>
    </body>
    </html>
    '''
