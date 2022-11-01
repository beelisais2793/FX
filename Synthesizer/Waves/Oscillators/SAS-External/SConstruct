# ENVIRONMENT
# FIXME choose flext sys with command line 
scons_env = Environment(CCFLAGS ='-O6 -fno-rtti -DFLEXT_SYS=2 -I/usr/include/pdextended',LIBS=['m','libflext-pd','libflext-pd_d','libflext-pd_s'],SHLIBPREFIX='')
        
# SOURCE FILES 
sas_lib = Split('''src/sas/fileio.c   
                src/sas/ieeefloat.c
                src/sas/sas_envelope.c
                src/sas/sas_frame.c
                src/sas/sas_file.c
                src/sas/sas_synthesizer.c
                src/sas/sas_file_format.c 
                src/sas/sas_file_msc_format.c 
                src/sas/sas_file_spectral.c
                src/sas/sas_file_partial.c
        ''')

sas_tilda = Split('src/sas_tilda.cpp')

scons_env.SharedLibrary('sas~.pd_linux',sas_lib+sas_tilda)

ext = scons_env.Install('/usr/local/lib/pd/extra', 'sas~.pd_linux')
help = scons_env.Install('/usr/local/lib/pd/extra', 'sas-help.pd')
scons_env.Alias('install', [ext,help])

