// ----------------------------------------------------------------------------
//
//  Copyright (C) 2003-2017 Fons Adriaensen <fons@linuxaudio.org>
//    
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------


#pragma once

#include <stdint.h>
#include <math.h>

#include <string.h>




namespace Reverb
{
    class Reverb;
    class Pareq
    {
    public:

        Pareq (void);
        ~Pareq (void);
        
        void setfsamp (DspFloatType fsamp);
        void setparam (DspFloatType f, DspFloatType g)
        {
        _f0 = f;
        _g0 = powf (10.0f, 0.05f * g);
        _touch0++;
        }
        void reset (void);
        void prepare (int nsamp);
        void process (int nsamp, int nchan, DspFloatType *data[])
        {
        if (_state != BYPASS) process1 (nsamp, nchan, data);
        }

    private:

        enum { BYPASS, STATIC, SMOOTH, MAXCH = 4 };

        void calcpar1 (int nsamp, DspFloatType g, DspFloatType f);
        void process1 (int nsamp, int nchan, DspFloatType *data[]);

        volatile int16_t  _touch0;
        volatile int16_t  _touch1;
        bool              _bypass;
        int               _state;
        DspFloatType             _fsamp;

        DspFloatType             _g0, _g1;
        DspFloatType             _f0, _f1;
        DspFloatType             _c1, _dc1;
        DspFloatType             _c2, _dc2;
        DspFloatType             _gg, _dgg;

        DspFloatType             _z1 [MAXCH];
        DspFloatType             _z2 [MAXCH];
    };



    // -----------------------------------------------------------------------


    class Diff1
    {
    private:

        friend class Reverb;
        
        Diff1 (void);
        ~Diff1 (void);

        void  init (int size, DspFloatType c);
        void  fini (void);

        DspFloatType process (DspFloatType x)
        {
        DspFloatType z = _line [_i];
        x -= _c * z;
            _line [_i] = x;
            if (++_i == _size) _i = 0;
        return z + _c * x;
        } 

        int     _i;
        DspFloatType   _c;
        int     _size;
        DspFloatType  *_line;
    };


    // -----------------------------------------------------------------------


    class Filt1
    {
    private:

        friend class Reverb;
        
        Filt1 (void) : _slo (0), _shi (0) {}
        ~Filt1 (void) {}

        void  set_params (DspFloatType del, DspFloatType tmf, DspFloatType tlo, DspFloatType wlo, DspFloatType thi, DspFloatType chi);

        DspFloatType process (DspFloatType x)
        {
            _slo += _wlo * (x - _slo) + 1e-10f;
            x += _glo * _slo;
            _shi += _whi * (x - _shi);
        return _gmf * _shi;
        }
        DspFloatType   _gmf;
        DspFloatType   _glo;
        DspFloatType   _wlo;
        DspFloatType   _whi;
        DspFloatType   _slo;
        DspFloatType   _shi;    
    };


    // -----------------------------------------------------------------------


    class Delay
    {
    private:

        friend class Reverb;
        
        Delay (void);
        ~Delay (void);

        void  init (int size);
        void  fini (void);

        DspFloatType read (void)
        {
        return _line [_i];
        }

        void write (DspFloatType x)
        {
        _line [_i++] = x;
        if (_i == _size) _i = 0;
        }

        int     _i;
        int     _size;
        DspFloatType  *_line;
    };


    // -----------------------------------------------------------------------


    class Vdelay
    {
    private:

        friend class Reverb;
        
        Vdelay (void);
        ~Vdelay (void);

        void  init (int size);
        void  fini (void);
        void  set_delay (int del);

        DspFloatType read (void)
        {
        DspFloatType x = _line [_ir++];
        if (_ir == _size) _ir = 0;
        return x;
        }

        void write (DspFloatType x)
        {
        _line [_iw++] = x;
        if (_iw == _size) _iw = 0;
        }

        int     _ir;
        int     _iw;
        int     _size;
        DspFloatType  *_line;
    };


    // -----------------------------------------------------------------------


    class Reverb
    {
    public:

        Reverb (void);
        ~Reverb (void);

        void init (DspFloatType fsamp, bool ambis);
        void fini (void);

        void prepare (int n);
        void process (int n, DspFloatType *inp [], DspFloatType *out []);

        void set_delay (DspFloatType v) { _ipdel = v; _cntA1++; }
        void set_xover (DspFloatType v) { _xover = v; _cntB1++; }
        void set_rtlow (DspFloatType v) { _rtlow = v; _cntB1++; }
        void set_rtmid (DspFloatType v) { _rtmid = v; _cntB1++; _cntC1++; }
        void set_fdamp (DspFloatType v) { _fdamp = v; _cntB1++; }
        void set_opmix (DspFloatType v) { _opmix = v; _cntC1++; }
        void set_rgxyz (DspFloatType v) { _rgxyz = v; _cntC1++; }
        void set_eq1 (DspFloatType f, DspFloatType g) { _pareq1.setparam (f, g); }
        void set_eq2 (DspFloatType f, DspFloatType g) { _pareq2.setparam (f, g); }

    private:


        DspFloatType   _fsamp;
        bool    _ambis;

        Vdelay  _vdelay0;
        Vdelay  _vdelay1;
        Diff1   _diff1 [8];
        Filt1   _filt1 [8];
        Delay   _delay [8];
        
        volatile int _cntA1;
        volatile int _cntB1;
        volatile int _cntC1;
        int     _cntA2;
        int     _cntB2;
        int     _cntC2;

        DspFloatType   _ipdel;
        DspFloatType   _xover;
        DspFloatType   _rtlow;
        DspFloatType   _rtmid;
        DspFloatType   _fdamp;
        DspFloatType   _opmix;
        DspFloatType   _rgxyz;

        DspFloatType   _g0, _d0;
        DspFloatType   _g1, _d1;

        Pareq   _pareq1;
        Pareq   _pareq2;

        static DspFloatType _tdiff1 [8];
        static DspFloatType _tdelay [8];
    };


    // -----------------------------------------------------------------------


    Pareq::Pareq (void) :
        _touch0 (0),
        _touch1 (0),
        _state (BYPASS),
        _g0 (1),
        _g1 (1),
        _f0 (1e3f),
        _f1 (1e3f)
    {
        setfsamp (0.0f);
    }


    Pareq::~Pareq (void)
    {
    }


    void Pareq::setfsamp (DspFloatType fsamp)
    {
        _fsamp = fsamp;
        reset ();
    }


    void Pareq::reset (void)
    {
        memset (_z1, 0, sizeof (DspFloatType) * MAXCH); 
        memset (_z2, 0, sizeof (DspFloatType) * MAXCH); 
    }


    void Pareq::prepare (int nsamp)
    {
        bool  upd = false;
        DspFloatType g, f;

        if (_touch1 != _touch0)
        {
        g = _g0;
        f = _f0;
            if (g != _g1)
        {
            upd = true;
            if      (g > 2 * _g1) _g1 *= 2;
            else if (_g1 > 2 * g) _g1 /= 2;
            else                  _g1 = g;
        }
            if (f != _f1)
        {
            upd = true;
            if      (f > 2 * _f1) _f1 *= 2;
            else if (_f1 > 2 * f) _f1 /= 2;
            else                  _f1 = f;
        }
        if (upd) 
        {
            if ((_state == BYPASS) && (_g1 == 1))
            {
            calcpar1 (0, _g1, _f1);
            }
            else
            {
            _state = SMOOTH;
            calcpar1 (nsamp, _g1, _f1);
            }
        }
        else
        {
            _touch1 = _touch0;
                if (fabs (_g1 - 1) < 0.001f)
                {
                _state = BYPASS;
                reset ();
            }
            else    
            {
                _state = STATIC;
            }
        }
        }
    }


    void Pareq::calcpar1 (int nsamp, DspFloatType g, DspFloatType f)
    {
        DspFloatType b, c1, c2, gg;

        f *= DspFloatType (M_PI) / _fsamp;
        b = 2 * f / sqrtf (g);         
        gg = 0.5f * (g - 1);
        c1 = -cosf (2 * f);
        c2 = (1 - b) / (1 + b);
        if (nsamp)
        {
        _dc1 = (c1 - _c1) / nsamp + 1e-30f;
        _dc2 = (c2 - _c2) / nsamp + 1e-30f;
        _dgg = (gg - _gg) / nsamp + 1e-30f;
        }
        else
        {
        _c1 = c1;
        _c2 = c2;
        _gg = gg;
        }
    }


    void Pareq::process1 (int nsamp, int nchan, DspFloatType *data[])
    {
        int   i, j;
        DspFloatType c1, c2, gg;
        DspFloatType x, y, z1, z2;
        DspFloatType *p;

        c1 = _c1;
        c2 = _c2;
        gg = _gg;
        if (_state == SMOOTH)
        {
        for (i = 0; i < nchan; i++)
        {
            p = data [i];
                z1 = _z1 [i];
                z2 = _z2 [i];
                c1 = _c1;
                c2 = _c2;
                gg = _gg;
                for (j = 0; j < nsamp; j++)
                {
                    c1 += _dc1;
                    c2 += _dc2;
                    gg += _dgg;
                x = *p;
                y = x - c2 * z2;
            *p++ = x - gg * (z2 + c2 * y - x);
                y -= c1 * z1;
                z2 = z1 + c1 * y;
                z1 = y + 1e-20f;
            }
                _z1 [i] = z1;
                _z2 [i] = z2;
        }
            _c1 = c1;
            _c2 = c2;
            _gg = gg;
        }
        else
        {
        for (i = 0; i < nchan; i++)
        {
            p = data [i];
                z1 = _z1 [i];
                z2 = _z2 [i];
                for (j = 0; j < nsamp; j++)
                {
                x = *p;
                y = x - c2 * z2;
            *p++ = x - gg * (z2 + c2 * y - x);
                y -= c1 * z1;
                z2 = z1 + c1 * y;
                z1 = y + 1e-20f;
            }
                _z1 [i] = z1;
                _z2 [i] = z2;
        }
        }
    }


    // -----------------------------------------------------------------------


    Diff1::Diff1 (void) :
        _size (0),
        _line (0)
    {
    }    


    Diff1::~Diff1 (void)
    {
        fini ();
    }


    void Diff1::init (int size, DspFloatType c)
    {
        _size = size;
        _line = new DspFloatType [size];
        memset (_line, 0, size * sizeof (DspFloatType));
        _i = 0;
        _c = c;
    }


    void Diff1::fini (void)
    {
        delete[] _line;
        _size = 0;
        _line = 0;
    }


    // -----------------------------------------------------------------------


    Delay::Delay (void) :
        _size (0),
        _line (0)
    {
    }


    Delay::~Delay (void)
    {
        fini ();
    }


    void Delay::init (int size)
    {
        _size = size;
        _line = new DspFloatType [size];
        memset (_line, 0, size * sizeof (DspFloatType));
        _i = 0;
    }


    void Delay::fini (void)
    {
        delete[] _line;
        _size = 0;
        _line = 0;
    }


    // -----------------------------------------------------------------------


    Vdelay::Vdelay (void) :
        _size (0),
        _line (0)
    {
    }


    Vdelay::~Vdelay (void)
    {
        fini ();
    }


    void Vdelay::init (int size)
    {
        _size = size;
        _line = new DspFloatType [size];
        memset (_line, 0, size * sizeof (DspFloatType));
        _ir = 0;
        _iw = 0;
    }


    void Vdelay::fini (void)
    {
        delete[] _line;
        _size = 0;
        _line = 0;
    }


    void Vdelay::set_delay (int del)
    {
        _ir = _iw - del;
        if (_ir < 0) _ir += _size;
    }


    // -----------------------------------------------------------------------


    void Filt1::set_params (DspFloatType del, DspFloatType tmf, DspFloatType tlo, DspFloatType wlo, DspFloatType thi, DspFloatType chi)
    {
        DspFloatType g, t;

        _gmf = powf (0.001f, del / tmf);
        _glo = powf (0.001f, del / tlo) / _gmf - 1.0f;
        _wlo = wlo;    
        g = powf (0.001f, del / thi) / _gmf;
        t = (1 - g * g) / (2 * g * g * chi);
        _whi = (sqrtf (1 + 4 * t) - 1) / (2 * t); 
    } 

    
    // -----------------------------------------------------------------------


    DspFloatType Reverb::_tdiff1 [8] = 
    {
        20346e-6f,
        24421e-6f,
        31604e-6f,
        27333e-6f,
        22904e-6f,
        29291e-6f,
        13458e-6f,
        19123e-6f
    };


    DspFloatType Reverb::_tdelay [8] = 
    {
    153129e-6f,
    210389e-6f,
    127837e-6f,
    256891e-6f,
    174713e-6f,
    192303e-6f,
    125000e-6f,
    219991e-6f
    };


    Reverb::Reverb (void)
    {
    }


    Reverb::~Reverb (void)
    {
        fini ();
    }

    /*
    R_DELAY, &r_delay   0.04);
    R_XOVER, &r_xover  200.0);
    R_RTLOW, &r_rtlow    3.0);
    R_RTMID, &r_rtmid    2.0);
    R_FDAMP, &r_fdamp  6.0e3);
    R_OPMIX, &r_opmix    0.5);
    R_RGXYZ, &r_rgxyz    0.0);
    R_EQ1FR, &r_parfr  160.0);
    R_EQ1GN, &r_pargn    0.0);
    R_EQ2FR, &r_parfr  2.5e3);
    R_EQ2GN, &r_pargn    0.0);
    */


    void Reverb::init (DspFloatType fsamp, bool ambis)
    {
        int i, k1, k2;

        _fsamp = fsamp;
        _ambis = ambis;
        _cntA1 = 1;
        _cntA2 = 0;
        _cntB1 = 1;
        _cntB2 = 0;
        _cntC1 = 1;
        _cntC2 = 0;

        _ipdel = 0.04f;
        _xover = 200.0f;
        _rtlow = 3.0f;
        _rtmid = 2.0f;
        _fdamp = 3e3f;
        _opmix = 0.5f;
        _rgxyz = 0.0f;

        _g0 = _d0 = 0;
        _g1 = _d1 = 0;

        _vdelay0.init ((int)(0.1f * _fsamp));
        _vdelay1.init ((int)(0.1f * _fsamp));
        for (i = 0; i < 8; i++)
        {
        k1 = (int)(floorf (_tdiff1 [i] * _fsamp + 0.5f));
        k2 = (int)(floorf (_tdelay [i] * _fsamp + 0.5f));
            _diff1 [i].init (k1, (i & 1) ? -0.6f : 0.6f);
            _delay [i].init (k2 - k1);
        }

        _pareq1.setfsamp (fsamp);
        _pareq2.setfsamp (fsamp);
    }


    void Reverb::fini (void)
    {
        for (int i = 0; i < 8; i++) _delay [i].fini ();
    }


    void Reverb::prepare (int nfram)
    {
        int    a, b, c, i, k;
        DspFloatType  t0, t1, wlo, chi;

        a = _cntA1;
        b = _cntB1;
        c = _cntC1;
        _d0 = _d1 = 0;

        if (a != _cntA2)
        {
            k = (int)(floorf ((_ipdel - 0.020f) * _fsamp + 0.5f));
            _vdelay0.set_delay (k);
            _vdelay1.set_delay (k);
            _cntA2 = a;
        }

        if (b != _cntB2)
        {
            wlo = 6.2832f * _xover / _fsamp;
        if (_fdamp > 0.49f * _fsamp) chi = 2;
        else chi = 1 - cosf (6.2832f * _fdamp / _fsamp);
            for (i = 0; i < 8; i++)
        {
                _filt1 [i].set_params (_tdelay [i], _rtmid, _rtlow, wlo, 0.5f * _rtmid, chi);
        }
            _cntB2 = b;
        }

        if (c != _cntC2)
        {
        if (_ambis)
        {
            t0 = 1.0f / sqrtf (_rtmid);
            t1 = t0 * powf (10.0f, 0.05f * _rgxyz);
        }
        else
        {
            t0 = (1 - _opmix) * (1 + _opmix);
            t1 = 0.7f * _opmix * (2 - _opmix) / sqrtf (_rtmid);
        }
            _d0 = (t0 - _g0) / nfram;
            _d1 = (t1 - _g1) / nfram;
            _cntC2 = c;
        }

        _pareq1.prepare (nfram);
        _pareq2.prepare (nfram);
    }


    void Reverb::process (int nfram, DspFloatType *inp [], DspFloatType *out [])
    {	
        int   i, n;
        DspFloatType *p0, *p1;
        DspFloatType *q0, *q1, *q2, *q3;
        DspFloatType t, g, x0, x1, x2, x3, x4, x5, x6, x7;

        g = sqrtf (0.125f);

        p0 = inp [0];
        p1 = inp [1];
        q0 = out [0];
        q1 = out [1];
        q2 = out [2];
        q3 = out [3];

        for (i = 0; i < nfram; i++)
        {
        _vdelay0.write (p0 [i]);
        _vdelay1.write (p1 [i]);

        t = 0.3f * _vdelay0.read ();
        x0 = _diff1 [0].process (_delay [0].read () + t);
        x1 = _diff1 [1].process (_delay [1].read () + t);
        x2 = _diff1 [2].process (_delay [2].read () - t);
        x3 = _diff1 [3].process (_delay [3].read () - t);
        t = 0.3f * _vdelay1.read ();
        x4 = _diff1 [4].process (_delay [4].read () + t);
        x5 = _diff1 [5].process (_delay [5].read () + t);
        x6 = _diff1 [6].process (_delay [6].read () - t);
        x7 = _diff1 [7].process (_delay [7].read () - t);

            t = x0 - x1; x0 += x1;  x1 = t;
            t = x2 - x3; x2 += x3;  x3 = t;
            t = x4 - x5; x4 += x5;  x5 = t;
            t = x6 - x7; x6 += x7;  x7 = t;
            t = x0 - x2; x0 += x2;  x2 = t;
            t = x1 - x3; x1 += x3;  x3 = t;
            t = x4 - x6; x4 += x6;  x6 = t;
            t = x5 - x7; x5 += x7;  x7 = t;
            t = x0 - x4; x0 += x4;  x4 = t;
            t = x1 - x5; x1 += x5;  x5 = t;
            t = x2 - x6; x2 += x6;  x6 = t;
            t = x3 - x7; x3 += x7;  x7 = t;

        if (_ambis)
        {
                _g0 += _d0;
                _g1 += _d1;
            q0 [i] = _g0 * x0;
            q1 [i] = _g1 * x1;
            q2 [i] = _g1 * x4;
            q3 [i] = _g1 * x2;
        }
        else
        {
                _g1 += _d1;
            q0 [i] = _g1 * (x1 + x2);
            q1 [i] = _g1 * (x1 - x2);
        }

        _delay [0].write (_filt1 [0].process (g * x0));
        _delay [1].write (_filt1 [1].process (g * x1));
        _delay [2].write (_filt1 [2].process (g * x2));
            _delay [3].write (_filt1 [3].process (g * x3));
            _delay [4].write (_filt1 [4].process (g * x4));
            _delay [5].write (_filt1 [5].process (g * x5));
            _delay [6].write (_filt1 [6].process (g * x6));
            _delay [7].write (_filt1 [7].process (g * x7));
        }

        n = _ambis ? 4 : 2;
        _pareq1.process (nfram, n, out);
        _pareq2.process (nfram, n, out);
        if (!_ambis)
        {
        for (i = 0; i < nfram; i++)
        {
            _g0 += _d0;
            q0 [i] += _g0 * p0 [i];
            q1 [i] += _g0 * p1 [i];
        }
        }
    }
}