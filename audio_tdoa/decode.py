import array

def decode(message):
    audio_id,timestamp,samples = array.array('H',message[:2]).tolist()[0], array.array('q',message[2:2+8]).tolist()[0], array.array('h',message[2+8:]).tolist()
    return dict(audio_id=audio_id, timestamp=timestamp, samples=samples)


