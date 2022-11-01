// GetOpt.h
// 

#ifndef GETOPT_H
#define GETOPT_H 

#include <string>
#include <stdexcept>
#include <sstream>
#include <cstring>

namespace soundmath {
	class GetOpt {
	private:
		GetOpt& operator= (GetOpt&);
		GetOpt (const GetOpt&);
	
		int m_sp;
		int	m_err;
		int	m_ind;
		int	m_opt;
		char* m_arg; 
		std::string m_format;
	public:
		GetOpt (const std::string& format) {
			m_format = format;
			reset ();
		}
		void reset () {
			m_sp = 1;
			m_err = 1;
			m_ind = 1;
			m_opt = 0;
			m_arg = 0;
		}
		int parse (int argc, char** argv) {
			int c;
			char *cp;
	
			if (m_sp == 1){
				if (m_ind >= argc ||
					argv[m_ind][0] != '-' || argv[m_ind][1] == '\0')
					return -1;
				else if (strcmp (argv[m_ind], "--") == 0) {
					m_ind++;
					return -1;
				}
			}
			m_opt = c = argv[m_ind][m_sp];
			if (c == ':' || (cp = strchr ((char*) m_format.c_str (), c)) == 0) {
				if (argv[m_ind][++m_sp] == '\0') {
					m_ind++;
					m_sp = 1;
				}
				std::stringstream msg;
				msg << "illegal option " << (char) c;
				throw std::runtime_error (msg.str ());
			}
			if (*++cp == ':') {
				if (argv[m_ind][m_sp+1] != '\0')
					m_arg = &argv[m_ind++][m_sp+1];
				else if (++m_ind >= argc) {
					m_sp = 1;
					std::stringstream msg;
					msg << "argument missing for " << (char) c;
					throw std::runtime_error (msg.str ());
				} else {
					m_arg = argv[m_ind++];
				}
				m_sp = 1;
			} else {
				if (argv[m_ind][++m_sp] == '\0') {
					m_sp = 1;
					m_ind++;
				}
				m_arg = 0;
			}
			return c;
		}
		int index () const {
			return m_ind;
		}
		int option () const {
			return m_opt;
		}
		char* arg () const {
			return m_arg;
		}
	};
}

#endif	// GETOPT_H 

// EOF
