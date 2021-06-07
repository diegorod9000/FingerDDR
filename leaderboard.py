from collections import OrderedDict
import sqlite3

def request_handler(request):
    # if "POST" in request['method']:
    #     song = request['values']['song']
    # get top 5 scores of each song 

    # connect to sql database 
    # find all songs 
    # get top 5 scores from all songs 
    # insert into to_return 
    
    db = "/var/jail/home/team43/fddr/lboard.db"
    conn = sqlite3.connect(db)
    c = conn.cursor()
    # (user text, score real, time real, song text) 

    songs = c.execute("""SELECT DISTINCT song from lboard_final""").fetchall()
    #return songs 
    sql_query = dict([])

    for song in songs: 
        row_dict = dict()
        values = c.execute("""SELECT * from lboard_final WHERE song = ? ORDER BY score DESC""", (song[0],)).fetchall()[:5]
        #return values
        for row in values: 
            row_dict[row[0]] = row[1]
            if song[0] in sql_query.keys():
                sql_query[song[0]].append(row) 
            else:
                sql_query[song[0]] = [row]
    #return sql_query   
    conn.commit()
    conn.close()
    #return sql_query
        
    to_return = """<!DOCTYPE html>
    <html>
    <body>
    <head>
        <link rel="stylesheet" href="leaderboard.css">
    </head>
    <div>
    <h1> Leaderboard </h1>
    <h2><a href="http://608dev-2.net/sandbox/sc/team43/fddr/game.py" class="button">Return to Homepage</a></h2>
    """ 

    if not bool(sql_query):
        to_return += """<h2><br>No top scorers yet today, you could be the first!</h2>"""
    else:
        for song in sql_query.keys():
            to_return += """<table width=100%><tr><th colspan="2">
             <h2><br>Top 5 Scores of """ + str(song) + """</h2>
          </th><tr><th class=\"right\">Name   </th><th class=\"left\">   Score</th></tr>"""

            for row in sql_query[song]:
                to_return += """<tr> <td class=\"right\">""" + str(row[0]) + """   </td>
                           <td class=\"left\">   """ + str(row[2]) + """</td>
                           </tr>"""

    #after for loop finish html thing:

    to_return +="""</div></body></html>"""
    return to_return
