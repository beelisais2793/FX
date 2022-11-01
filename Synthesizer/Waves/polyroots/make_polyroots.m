%MAKE_POLYROOTS Compilation of the POLYROOTS mex-file
MATLAB_PATH = matlabroot;
COMPILE_OPTIONS = '';
v = ver('matlab');
matver = sscanf(v.Version, '%d.%d.%d')';
COMPILE_OPTIONS = [ COMPILE_OPTIONS ' -DMATLAB_VERSION=0x' sprintf('%02d%02d', matver(1), matver(2)) ];
MATLAB_VERSION = matver(1) + matver(2)/100;

if strcmpi('GLNX86', computer) || strcmpi('GLNXA64', computer) ...
        || strcmpi('MACI', computer) || strcmpi('MAC', computer) ...
        || strcmpi('MACI64', computer)
    % GNU/Linux (x86-32 or x86-64) or MacOS (Intel or PPC)
    if strcmpi('GLNX86', computer) || strcmpi('GLNXA64', computer)
        COMPILE_OPTIONS = [COMPILE_OPTIONS,' -DSkip_f2c_Undefs',' -DNON_UNIX_STDIO'];
    else
        COMPILE_OPTIONS = [COMPILE_OPTIONS,' -DSkip_f2c_Undefs'];
    end
elseif strcmpi('PCWIN', computer) || strcmpi('PCWIN64', computer)
    % Windows (x86-32 or x86-64)
    if strcmpi('PCWIN', computer)
        if MATLAB_VERSION < 7.06
            MANUFACTURER = 'lcc';
        else
            cc = mex.getCompilerConfigurations('C','Selected');
            MANUFACTURER = cc.Manufacturer;
        end
        switch lower(MANUFACTURER)
            case {'lcc'}
                COMPILE_OPTIONS = [COMPILE_OPTIONS,' -DLCCWIN32'];
            case {'microsoft'}
            case {'gnu'}
            case {'sybase'}
                COMPILE_OPTIONS = [COMPILE_OPTIONS,' -DWATCOMWIN32'];
            otherwise
                disp('Try "mex -setup"!')
        end
    else
        cc = mex.getCompilerConfigurations('C','Selected');
        MANUFACTURER = cc.Manufacturer;
        switch lower(MANUFACTURER)
            case {'microsoft'}
            case {'gnu'}
            otherwise
                disp('Try "mex -setup"!')
        end
    end
else
    error('Unsupported platform')
end

% Large array dims for 64 bits platforms appeared in Matlab 7.3
if (strcmpi('GLNXA64', computer) || strcmpi('PCWIN64', computer) ...
        || strcmpi('MACI64', computer))
    if ~(MATLAB_VERSION < 9.04)
        COMPILE_OPTIONS = [ COMPILE_OPTIONS ' -R2017b' ];
    elseif ~(MATLAB_VERSION < 7.03)
        COMPILE_OPTIONS = [ COMPILE_OPTIONS ' -largeArrayDims' ];
    end
end

% Comment next line to suppress optimization
COMPILE_OPTIONS = [ ' -O' COMPILE_OPTIONS ];

% Comment next line to suppress compilation debugging info
%COMPILE_OPTIONS = [ ' -v' COMPILE_OPTIONS ];

disp('Compiling polyroots...')
eval(['mex ', COMPILE_OPTIONS, ' -Ilibf2c', ' polyroots.c',' acm419/cpoly.c',' acm439/rpoly.c',' libf2c/pow_di.c']);
