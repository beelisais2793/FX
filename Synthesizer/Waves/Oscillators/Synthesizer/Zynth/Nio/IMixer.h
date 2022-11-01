#ifndef _IMIXER_H_
#define _IMIXER_H_

class IMixer
{
protected:
    IMixer() { }

public:
    virtual ~IMixer() { }

    virtual void AudioOut(float *outl, float *outr) = 0;
    virtual void Lock() = 0;    //pthread_mutex_lock(&(master.mutex));
    virtual void UnLock() = 0;  //pthread_mutex_unlock(&(master.mutex));

    virtual void NoteOn(char chan, char note, char velocity) = 0;
    virtual void NoteOff(char chan, char note) = 0;
    virtual void PolyphonicAftertouch(char chan, char note, char velocity) = 0;
    virtual void SetController(char chan, int type, int par) = 0;
    virtual void SetProgram(char chan, unsigned int pgm) = 0;
};

#endif // _IMIXER_H_
