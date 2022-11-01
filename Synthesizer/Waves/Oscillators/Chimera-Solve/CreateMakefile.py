#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

# Variable definition
makefile = 'Makefile'

executable = 'chimera'
extend_library = 'module'
extend_library_name = 'lib' + extend_library + '.so'
core_library = 'chimera'
core_library_name = 'lib' + core_library + '.so'
runtime_library = 'runtime'
runtime_library_name = 'lib' + runtime_library + '.so'
lua_path = 'lua-5.3.3'
lua_src_path = os.path.join(lua_path, 'src')
lua_lib = 'liblua.so.5.3.3'
lua_lib_ln = 'liblua.so'

part_main = 'main'
part_runtime = 'runtime'
part_extend = 'extend'
part_core = 'core'
part_module = 'module'

flags = {
	part_main: {
		'inc': ['-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-ldl', '-lboost_system', '-lboost_filesystem']
	},
	part_runtime: {
		'inc': ['-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-fPIC', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-ldl', '-lboost_system', '-lboost_filesystem', '-shared', '-Wl,-soname']
	},
	part_extend: {
		'inc': ['-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-fPIC', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-lboost_system', '-shared', '-Wl,-soname']
	},
	part_module: {
		'inc': ['-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-fPIC', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-lboost_system', '-shared', '-Wl,-soname']
	},
	part_core: {
		'inc': ['-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-fPIC', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-Lbin/Debug', '-llua', '-lboost_system', '-ldl', '-shared', '-Wl,-soname']
	}
}

built_names = {
    part_main: '$(EXE_NAME)',
	part_runtime: '$(RUN_NAME)',
	part_extend: '$(MOD_NAME)',
	part_core: '$(CORE_NAME)',
	part_module: ''
}

path_names = {
	part_runtime: 'runtime',
	part_extend: 'extend',
	part_main: 'main',
	part_core: 'core',
	part_module: 'modules'
}

relate_names = {
	part_main: 'out',
	part_runtime: 'run',
	part_extend: 'mod',
	part_core: 'core',
	part_module: ''
}

inc_names = {
	part_runtime: runtime_library,
	part_extend: extend_library,
	part_core: core_library
}

dependencies = {
	part_main: [part_runtime, part_extend, part_core],
	part_runtime: [part_extend, part_core],
	part_extend: [part_core],
	part_core: [],
	part_module: [part_extend, part_core]
}

phony = {
	'unique': ['clean', 'install', 'uninstall'],
	'target': ['before', 'after', 'clean']
}

# Add include paths for all dependencies
for fname in flags:
	flags[fname]['inc'].append('-I' + path_names[fname] + '/include')
	for d in dependencies[fname]:
		flags[fname]['inc'].append('-I' + path_names[d] + '/include')

default_target = 'release'
targets = ['debug', 'release']

def createTarget(f, target, files, modules):
	f.write('{0}: before_{0} lua_{0} {3}_{0} $({2}_OBJ_{1}) after_{0}\n\n'.format(target, target.upper(), part_module.upper(), relate_names[part_main]))
	beforeTarget(f, target)
	luaTarget(f, target)
	libTarget(f, target, part_runtime)
	libTarget(f, target, part_extend)
	libTarget(f, target, part_core)
	for m in modules:
		moduleTarget(f, target, m, modules[m])
	outTarget(f, target)

	for ft in files:
		for filedict in files[ft]:
			srcPath = filedict['src']
			objPath = filedict['obj']
			cxxFileTarget(f, target, srcPath, objPath, ft)
			#if ft == part_module:
			#	ldFileTarget(f, target, filedict['name'], objPath, ft)
			f.write('\n')

	afterTarget(f, target)
	cleanTarget(f, target)


def beforeTarget(f, target):
	f.write('before_' + target + ':\n')
	for c in path_names:
		f.write('	test -d $(' + c.upper() + '_OUT_' + target.upper() + ') || mkdir -p $(' + c.upper() + '_OUT_' + target.upper() + ')\n')
		f.write('	test -d $(' + c.upper() + '_OBJDIR_' + target.upper() + ') || mkdir -p $(' + c.upper() + '_OBJDIR_' + target.upper() + ')\n')
	f.write('\n')

def luaTarget(f, target):
	f.write('lua_{0}: lua\n'.format(target))
	f.write('	cp -u {2} $({1}_OUT_{0})\n'.format(target.upper(), part_main.upper(), os.path.join(lua_src_path, lua_lib)))
	f.write('	cd $({1}_OUT_{0}) && ln -sf {2} {3}\n'.format(target.upper(), part_main.upper(), lua_lib, lua_lib_ln))
	f.write('\n')

def libTarget(f, target, part_name):
	str_dep = ''
	str_inc = ''
	for dep_n in dependencies[part_name]:
		str_dep += '{1}_{0} '.format(target, relate_names[dep_n])
		#str_inc += '-l' + inc_names[part_name] + ' '
	f.write(('{3}_{0}: before_{0} ' + str_dep + ' $({2}_OBJ_{1})\n').format(target, target.upper(), part_name.upper(), relate_names[part_name]))
	f.write(('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUTFILE_{0}) $({1}_OBJ_{0}) ' + str_inc + ' $({1}_LDFLAGS_{0}),{2} $({1}_LIB_{0})\n').format(target.upper(), part_name.upper(), built_names[part_name]))
	f.write('\n')

def outTarget(f, target):
	str_dep = ''
	str_inc = ''
	for dep_n in dependencies[part_main]:
		str_dep += '{1}_{0} '.format(target, relate_names[dep_n])
		str_inc += '-l' + inc_names[dep_n] + ' '
	f.write(('{3}_{0}: before_{0} ' + str_dep + ' $({2}_OBJ_{1})\n').format(target, target.upper(), part_main.upper(), relate_names[part_main]))
	f.write(('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUTFILE_{0}) $({1}_OBJ_{0}) ' + str_inc + ' $({1}_LDFLAGS_{0}) $({1}_LIB_{0})\n').format(target.upper(), part_main.upper()))
	f.write('\n')

def cxxFileTarget(f, target, src_file, obj_file, part):
	f.write('$({1}_OBJDIR_{0})/{3}: {4}/src/{2}\n'.format(target.upper(), part.upper(), src_file, obj_file, path_names[part]))
	f.write('	$(CXX) $({1}_CFLAGS_{0}) $({1}_INC_{0}) -c {4}/src/{2} -o $({1}_OBJDIR_{0})/{3}\n'.format(target.upper(), part.upper(), src_file, obj_file, path_names[part]))

def ldFileTarget(f, target, name, obj_file, part):
	f.write('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUT_{0})/{2}.mod $({1}_OBJDIR_{0})/{3} $({1}_LDFLAGS_{0}),{2}.mod $({1}_LIB_{0})\n'.format(target.upper(), part.upper(), name, obj_file))

def moduleTarget(f, target, name, files):
	str_objs = ''
	for fi in files:
		str_objs += ('$({1}_OBJDIR_{0})/{2} ').format(target.upper(), part_module.upper(), fi['obj'])
	target_name = part_module.lower() + '_' + name.lower() + '_' + target.lower()
	f.write(target_name + ': ' + str_objs + '\n')
	f.write(('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUT_{0})/{2}.mod ' + str_objs + ' $({1}_LDFLAGS_{0}),{2}.mod $({1}_LIB_{0})\n\n').format(target.upper(), part_module.upper(), name))


def afterTarget(f, target):
	f.write('after_' + target + ':\n'.format(target.upper(), part_runtime.upper()))
	f.write('	cp test.lua $({1}_OUT_{0})/test.lua\n'.format(target.upper(), part_runtime.upper()))
	f.write('	cp run.sh $({1}_OUT_{0})/run.sh\n'.format(target.upper(), part_runtime.upper()))
	f.write('\n')

def cleanTarget(f, target):
	f.write('clean_' + target + ':\n')
	for c in path_names:
		f.write('	rm -rf $({1}_OBJDIR_{0})\n'.format(target.upper(), c.upper()))
		f.write('	rm -rf $({1}_OUT_{0})\n'.format(target.upper(), c.upper()))
	f.write('\n')

with open(makefile, 'w') as f:
	f.write('#------------------------------------------------------------------------------#\n')
	f.write('# This makefile was created with python                                        #\n')
	f.write('#------------------------------------------------------------------------------#\n')
	f.write('\n')
	f.write('WORKDIR = `pwd`\n')
	f.write('\n')
	f.write('CC = gcc\n')
	f.write('CXX = g++\n')
	f.write('AR = ar\n')
	f.write('LD = g++\n')
	f.write('WINDRES = windres\n')
	f.write('\n')

	# set all common definitions
	for c in flags:
		for item in flags[c]:
			f.write(c.upper() + '_' + item.upper() + ' =')
			for flag in flags[c][item]:
				f.write(' ' + flag)
			f.write('\n')
		f.write('\n')

	# built names
	f.write('EXE_NAME = ' + executable + '\n')
	f.write('MOD_NAME = ' + extend_library_name + '\n')
	f.write('RUN_NAME = ' + runtime_library_name + '\n')
	f.write('CORE_NAME = ' + core_library_name + '\n')

	f.write('\n')

	# Target definitions
	for t in targets:
		targetName = t[0].upper() + t[1:].lower()
		for c in flags:
			for item in flags[c]:
				f.write(c.upper() + '_' + item.upper() + '_' + t.upper() + ' =')
				if t.lower() == 'release' and item.lower() == 'ldflags':
					f.write(' -s')
				f.write(' $(' + c.upper() + '_' + item.upper() + ')')
				if t.lower() == 'debug' and item.lower() == 'cflags':
					f.write(' -g')
				elif t.lower() == 'release' and item.lower() == 'cflags':
					f.write(' -O2')
				if item.lower() == 'ldflags' and c == part_main:
					f.write(' -Lbin/' + targetName + ' -l' +extend_library + ' -llua')
				#if item.lower() == 'ldflags' and c == part_extend:
				#	f.write(' -Lbin/' + targetName + ' -llua')
				f.write('\n')

			f.write(c.upper() + '_OBJDIR_' + t.upper() + ' = obj/' + targetName + '/' + c + '\n')
			if c != part_module:
				f.write(c.upper() + '_OUT_' + t.upper() + ' = bin/' + targetName + '\n')
			else:
				f.write(c.upper() + '_OUT_' + t.upper() + ' = bin/' + targetName + '/modules\n')
			f.write(c.upper() + '_OUTFILE_' + t.upper() + ' = bin/' + targetName + '/' + built_names[c] + '\n')

			f.write('\n')
		f.write('\n')

	# Find all files recursively
	src_files = {}
	def checkSrcPath(root, path, files):
		lpath = root if path is None else os.path.join(root, path)
		for o in os.listdir(lpath):
			if os.path.isfile(os.path.join(lpath, o)) and o.rfind('.cpp') == len(o) - 4:
				on = o.replace('.cpp', '')
				files.append({
					'name': on,
					'path': "" if path is None else path,
					'src': o if path is None else os.path.join(path, o),
					'obj': on + ".o"
				})
			if os.path.isdir(os.path.join(lpath, o)):
				dpath = o if path is None else os.path.join(path, o)
				checkSrcPath(root, dpath, files)

	# Find all files for built and libs
	for pn in path_names:
		src_files[pn] = []
		checkSrcPath(os.path.join(path_names[pn], 'src'), None, src_files[pn])

	# find structure for all modules
	module_names = {}
	lpath = os.path.join(path_names[part_module], 'src')
	for o in os.listdir(lpath):
		if os.path.isfile(os.path.join(lpath, o)) and o.rfind('.cpp') == len(o) - 4:
			on = o.replace('.cpp', '')
			module_names[on] = [{
				'name': on,
				'path': lpath,
				'src': os.path.join(lpath, o),
				'obj': on + ".o"
			}]
		if os.path.isdir(os.path.join(lpath, o)):
			module_names[o] = []
			checkSrcPath(lpath, o, module_names[o])

	for t in targets:
		for c in src_files:
			f.write(c.upper() + '_OBJ_' + t.upper() + ' =')
			objdir = '$(' + c.upper() + '_OBJDIR_' + t.upper() + ')/'
			if c != part_module:
				for sf in src_files[c]:
					f.write(' ' + objdir + sf['obj'])
				f.write('\n')
			else:
				for mn in module_names:
					f.write(' ' + c.lower() + '_' + mn.lower() + '_' + t.lower())
				f.write('\n')

		f.write('\n')

	f.write('\n')
	createTarget(f, default_target, src_files, module_names)
	for t in targets:
		if t == default_target:
			continue
		createTarget(f, t, src_files, module_names)

	f.write('lua:\n')
	f.write('\tcd lua-5.3.3 && $(MAKE) linux\n\n')

	f.write('clean:')
	for t in targets:
		f.write(' clean_' + t)
	f.write('\n\n')

	f.write('install:\n')
	f.write('\ttest -d bin/Release || exit 1\n')
	f.write('\ttest -d /usr/local/bin || mkdir -p /usr/local/bin\n')
	f.write('\tcp bin/Release/$(EXE_NAME) /usr/local/bin/$(EXE_NAME)\n')
	f.write('\tchmod +x /usr/local/bin/$(EXE_NAME)\n')
	f.write('\ttest -d /usr/local/lib || mkdir -p /usr/local/lib\n')
	f.write('\tcp bin/Release/*.so /usr/local/lib/\n')
	f.write('\tldconfig\n')
	f.write('\tcp -R bin/Release/modules /usr/local/lib/chimera-modules\n')
	f.write('\ttest -d /etc/chimera || mkdir -p /etc/chimera\n')
	f.write('\techo "LogLevel=Error" > /etc/chimera/solver.ini\n')
	f.write('\techo "" >> /etc/chimera/solver.ini\n')
	f.write('\techo "[Filesystem]" >> /etc/chimera/solver.ini\n')
	f.write('\techo "type=filesystem" >> /etc/chimera/solver.ini\n')
	f.write('\techo "path=/usr/local/lib/chimera-modules" >> /etc/chimera/solver.ini\n')
	f.write('\n')
	
	f.write('uninstall:\n')
	f.write('\trm -rf /usr/local/bin/chimera\n')
	f.write('\trm -rf /usr/local/lib/chimera-modules\n')
	f.write('\trm -rf /usr/local/lib/libchimera.so\n')
	f.write('\trm -rf /usr/local/lib/liblua.so\n')
	f.write('\trm -rf /usr/local/lib/libmodule.so\n')
	f.write('\trm -rf /usr/local/lib/libruntime.so\n')
	f.write('\tldconfig\n')
	f.write('\n')


	f.write('.PHONY:')
	for pt in phony['unique']:
		f.write(' ' + pt)

	for t in targets:
		for pt in phony['target']:
			f.write(' ' + pt + '_' + t)
