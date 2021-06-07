import sys
import os
import sqlite3
import datetime
from bokeh.plotting import figure
from bokeh.embed import components

home = "/var/jail/home/team43/fddr/"
sys.path.append(home)
from maps.map_creator import make_beat_map

ht_db = '/var/jail/home/team43/databases/maps.db'

USERS = ['acheerla', 'acheerla_friend'] #update

def request_handler(request):
    if request['method'] == "POST":
        with sqlite3.connect(ht_db) as c:
            if 'song' not in request['form']:
                return "Invalid POST request."
            
            song_name = request['form']['song']
            loaded = c.execute("""SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?;""", [song_name]).fetchall()[0][0]
            
            if(loaded):
                # return str(song_name) + " has already been already loaded!"
                c.execute("DELETE FROM "+song_name+";")
            
            song_dir = os.path.join(home, "songs", str(song_name) + ".txt")
            try:
                notes = make_beat_map(song_dir)
            except: 
                return song_dir
                # return str(song_name) + ".txt not found!"
            c.execute("CREATE TABLE IF NOT EXISTS "+song_name+" (note real, time real)")
            for note in notes: 
                c.execute("INSERT into " + song_name + " VALUES (?,?);", (note[0], note[1]))
        return str(song_name) + " successfully loaded!"

    if request['method'] =="GET":
            if 'song' not in request['values'] or 'note' not in request['values']:
                return "Invalid GET request"
            song_name = request['values']['song']
            note_type = request['values']['note']


            with sqlite3.connect(ht_db) as c:
                loaded = c.execute("""SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?;""", [song_name]).fetchall()[0][0]
                if(not loaded):
                    return "Song not yet loaded"
                
                # get values from time_since_start to 10 ahead 
                next_ten_notes = c.execute("SELECT * FROM " + song_name + " WHERE note = ? ;",(note_type,)).fetchall()
                
                output = []
                for note in next_ten_notes:
                    output.append(int(note[1]))
                
                return str(output)

             
            # if cmd = load 
            # check if already loaded by searching for table in maps.db
            # if not loaded, call map_generator.py on song_name.txt (if doesnt exist, error)
            # store result of map_generator.py in database 

            # if cmd = query 
                # get next 10 seconds from song_name.db 
                # ouput as comma, semicolon seperated list on postman
