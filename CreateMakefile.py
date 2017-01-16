#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

makefile = 'Makefile'

executable = 'NetworkSimulation'
library = 'module'
library_name = 'lib' + library + '.so'
lua_path = 'lua-5.3.3'
lua_src_path = os.path.join(lua_path, 'src')
lua_lib = 'liblua.so.5.3.3'
lua_lib_ln = 'liblua.so'
part_main = 'main'
part_extend = 'extend'
part_module = 'module'

flags = {
	part_main: {
		'inc': ['-Icore/include', '-Iextend/include', '-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-ldl', '-lboost_system', '-lboost_filesystem']
	},
	part_extend: {
		'inc': ['-Iextend/include', '-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-fPIC', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-shared', '-Wl,-soname']
	},
	part_module: {
		'inc': ['-Imodules/include', '-Iextend/include', '-Ilua-5.3.3/src'],
		'cflags': ['-Wall', '-fexceptions', '-fPIC', '-std=c++11'],
		'resinc': [],
		'libdir': [],
		'lib': [],
		'ldflags': ['-lboost_system', '-shared', '-Wl,-soname']
	}
}

built_names = {
	part_main: '$(EXE_NAME)',
	part_extend: '$(MOD_NAME)',
	part_module: ''
}

path_names = {
	part_main: 'core',
	part_extend: 'extend',
	part_module: 'modules'
}

phony = {
	'unique': ['clean'],
	'target': ['before', 'after', 'clean']
}

default_target = 'release'
targets = ['debug', 'release']

def createTarget(f, target, files):
	f.write('{0}: before_{0} lua_{0} mod_{0} out_{0} $({2}_OBJ_{1}) after_{0}\n\n'.format(target, target.upper(), part_module.upper()))
	beforeTarget(f, target)
	luaTarget(f, target)
	modTarget(f, target)
	outTarget(f, target)

	for ft in files:
		for filedict in files[ft]:
			srcPath = filedict['src']
			objPath = filedict['obj']
			cxxFileTarget(f, target, srcPath, objPath, ft)
			if ft == part_module:
				ldFileTarget(f, target, filedict['name'], objPath, ft)
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

def modTarget(f, target):
	f.write('mod_{0}: before_{0} $({2}_OBJ_{1})\n'.format(target, target.upper(), part_extend.upper()))
	f.write('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUTFILE_{0}) $({1}_OBJ_{0}) $({1}_LDFLAGS_{0}),$(MOD_NAME) $({1}_LIB_{0})\n'.format(target.upper(), part_extend.upper()))
	f.write('\n')

def outTarget(f, target):
	f.write('out_{0}: before_{0} $({2}_OBJ_{1})\n'.format(target, target.upper(), part_main.upper()))
	f.write('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUTFILE_{0}) $({1}_OBJ_{0}) $({1}_LDFLAGS_{0}) $({1}_LIB_{0})\n'.format(target.upper(), part_main.upper()))
	f.write('\n')

def cxxFileTarget(f, target, src_file, obj_file, part):
	f.write('$({1}_OBJDIR_{0})/{3}: {4}/src/{2}\n'.format(target.upper(), part.upper(), src_file, obj_file, path_names[part]))
	f.write('	$(CXX) $({1}_CFLAGS_{0}) $({1}_INC_{0}) -c {4}/src/{2} -o $({1}_OBJDIR_{0})/{3}\n'.format(target.upper(), part.upper(), src_file, obj_file, path_names[part]))

def ldFileTarget(f, target, name, obj_file, part):
	f.write('	$(LD) $({1}_LIBDIR_{0}) -o $({1}_OUT_{0})/{2}.mod $({1}_OBJDIR_{0})/{3} $({1}_LDFLAGS_{0}),{2}.mod $({1}_LIB_{0})\n'.format(target.upper(), part.upper(), name, obj_file))

def afterTarget(f, target):
	f.write('after_' + target + ':\n'.format(target.upper(), part_main.upper()))
	f.write('	cp test.lua $({1}_OUT_{0})/test.lua\n'.format(target.upper(), part_main.upper()))
	f.write('	cp run.sh $({1}_OUT_{0})/run.sh\n'.format(target.upper(), part_main.upper()))
	f.write('\n')

def cleanTarget(f, target):
	f.write('clean_' + target + ':\n')
	for c in path_names:
		f.write('	rm -rf $({1}_OBJDIR_{0})\n'.format(target.upper(), c.upper()))
		f.write('	rm -rf $({1}_OUT_{0})\n'.format(target.upper(), c.upper()))
	f.write('\n')

with open(makefile, 'w') as f:
	f.write('#------------------------------------------------------------------------------#\n')
	f.write('# This makefile was with python                                                #\n')
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

	for c in flags:
		for item in flags[c]:
			f.write(c.upper() + '_' + item.upper() + ' =')
			for flag in flags[c][item]:
				f.write(' ' + flag)
			f.write('\n')
		f.write('\n')

	f.write('EXE_NAME = ' + executable + '\n')
	f.write('MOD_NAME = ' + library_name + '\n')
	f.write('\n')

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
					f.write(' -Lbin/' + targetName + ' -l' +library + ' -llua')
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

	for pn in path_names:
		src_files[pn] = []
		checkSrcPath(os.path.join(path_names[pn], 'src'), None, src_files[pn])

	for t in targets:
		for c in src_files:
			f.write(c.upper() + '_OBJ_' + t.upper() + ' =')
			objdir = '$(' + c.upper() + '_OBJDIR_' + t.upper() + ')/'
			for sf in src_files[c]:
				f.write(' ' + objdir + sf['obj'])
			f.write('\n')
		f.write('\n')

	f.write('\n')
	createTarget(f, default_target, src_files)
	for t in targets:
		if t == default_target:
			continue
		createTarget(f, t, src_files)

	f.write('lua:\n')
	f.write('\tcd lua-5.3.3 && $(MAKE) linux\n\n')

	f.write('clean:')
	for t in targets:
		f.write(' clean_' + t)
	f.write('\n\n')

	f.write('.PHONY:')
	for pt in phony['unique']:
		f.write(' ' + pt)

	for t in targets:
		for pt in phony['target']:
			f.write(' ' + pt + '_' + t)
