/* $Id$ */
#include "stdafx.h"

#include "roengine/file_manager.h"

#include <sys/stat.h>

FileLoader::FileLoader() {

}

FileLoader::~FileLoader() {

}

bool FileLoader::writeFile(const std::string& name, std::ostream& os) {
	FileData file = getFile(name);
	return(file.write(os));
}

bool GRFFileLoader::fileExists(const std::string& name) const {
	std::string fn, aux = "data\\";
	for (unsigned int i = 0; i < name.length(); i++)
		if (name[i] >= 'A' && name[i] <= 'Z')
			fn += (name[i] - 'A' + 'a');
		else
			fn += name[i];

	fn = aux + fn;

	return(m_grf.fileExists(fn));
}

FileData GRFFileLoader::getFile(const std::string& name) {
	FileData ret;
	std::stringstream ss;
	std::string fn, aux = "data\\";
	for (unsigned int i = 0; i < name.length(); i++)
		if (name[i] >= 'A' && name[i] <= 'Z')
			fn += (name[i] - 'A' + 'a');
		else
			fn += name[i];

	fn = aux + fn;

	m_grf.write(fn, ss);
	ret.setSize(ss.tellp());
	ss.read((char*)*ret, ss.tellp());
	return(ret);
}

bool GRFFileLoader::open(const std::string& name) {
	return(m_grf.open(name));
}

bool FSFileLoader::fileExists(const std::string& name) const {
	std::string fn = m_path;
	fn += name;
	std::string::iterator it = fn.begin() + m_path.length();
	for (; it != fn.end(); ++it)
		if( *it == '\\' || *it == '/' )
			*it = DIR_SEPARATOR;

	struct stat stFileInfo;
	if (stat(fn.c_str(), &stFileInfo) != 0)
		return(false);// not found
	if ((stFileInfo.st_mode&S_IFDIR) != 0)
		return(false);// directory

	return(true);
}

FileData FSFileLoader::getFile(const std::string& name) {
	std::ifstream file;
	FileData ret;

	std::string fn = m_path;
	fn += name;
	std::string::iterator it = fn.begin() + m_path.length();
	for (; it != fn.end(); ++it)
		if( *it == '\\' || *it == '/' )
			*it = DIR_SEPARATOR;

	file.open(fn.c_str(), std::ios_base::in | std::ios_base::binary);

	if(!file.is_open())
		return(ret);

	file.seekg(0, std::ios_base::end);
	int size = file.tellg();
	file.seekg(0, std::ios_base::beg);
	ret.setSize(size);
	file.read(ret.getBuffer(), size);

	return(ret);
}
bool FSFileLoader::open(const std::string& name) {
	struct stat stDirInfo;
	if (stat(name.c_str(), &stDirInfo) != 0)
		return(false);// not found
	if ((stDirInfo.st_mode&S_IFDIR) == 0)
		return(false);// not directory

	m_path = name;
	if (m_path[m_path.length() - 1] != DIR_SEPARATOR)
		m_path += DIR_SEPARATOR;
	return(true);
}


FileManager::FileManager() {
}

FileManager::FileManager(const std::string& ini) {
	// Reads information from an INI file
	LoadIni(ini);
}

bool FileManager::LoadIni(const std::string& ini) {
	return(true);
}

bool FileManager::add(const std::string& name, FileLoader* loader) {
	if (!BaseCache<FileLoader>::add(name, loader))
		return(false);
	LoadOrder.push_back(name);
	return(true);
}

bool FileManager::remove(const std::string& name) {
	if (!BaseCache<FileLoader>::remove(name))
		return(false);
	std::vector<std::string>::iterator itr;
	itr = LoadOrder.begin();
	while(itr != LoadOrder.end()) {
		if ((*itr) == name) {
			LoadOrder.erase(itr);
			return(true);
		}
		itr++;
	}

	return(true);
}

bool FileManager::OpenGRF(const std::string& grf_fn) {
	std::string grfn = "grf:";
	grfn += grf_fn;

	if (exists(grfn))
		return(false);

	GRFFileLoader *m_grf = new GRFFileLoader();
	if (!m_grf->open(grf_fn)) {
		delete(m_grf);
		return(false);
	}

	add(grfn, m_grf);
	LoadOrder.push_back(grfn);
	return(true);
}

bool FileManager::OpenFS(const std::string& location) {
	std::string fnn = "fs:";
	fnn += location;

	if (exists(fnn))
		return(false);

	FSFileLoader *m_fs = new FSFileLoader();
	if (!m_fs->open(location)) {
		delete(m_fs);
		return(false);
	}

	add(fnn, m_fs);
	LoadOrder.push_back(fnn);
	return(true);
}

bool FileManager::fileExists(const std::string& n) const {
	std::string name;
	std::map<std::string, std::string>::const_iterator mapitr;

	mapitr = nametable.find(n);
	if (mapitr != nametable.end()) {
		name = mapitr->second;
	}
	else {
		name = n;
	}

	std::vector<std::string>::const_iterator itr;
	itr = LoadOrder.begin();
	const FileLoader *fl;
	while (itr != LoadOrder.end()) {
		fl = get(*itr);
		if (fl->fileExists(name))
			return(true);
		itr++;
	}
	return(false);
}

FileData FileManager::getFile(const std::string& n) {
	std::string name;
	std::map<std::string, std::string>::const_iterator mapitr;

	mapitr = nametable.find(n);
	if (mapitr != nametable.end()) {
		name = mapitr->second;
	}
	else {
		name = n;
	}

	std::vector<std::string>::const_iterator itr;
	FileData ret;
	itr = LoadOrder.begin();
	FileLoader *fl;
	while (itr != LoadOrder.end()) {
		fl = get(*itr);
		if (fl->fileExists(name)) {
			ret = fl->getFile(name);
			return(ret);
		}
		itr++;
	}

	return(ret);
}

void FileManager::addName(const std::string& a, const std::string& b) {
	nametable[a] = b;
}
