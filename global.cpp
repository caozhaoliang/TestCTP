#include "include/global.h"
#include "../version.h"
bool dirExist(const std::string& path){
	return access(path.c_str(),F_OK)==0;
}

bool makedirs(const std::string& path){
	std::string strCmd = "mkdir -p "+path;
	return system(strCmd.c_str());
}

bool rmdirs(const std::string& path){
	DIR * pDir = NULL;  
	struct dirent *pDirent = NULL; 

	if((pDir = opendir(path.c_str())) == NULL){
		return false;
	}

	char szBuf[256] = {0};
	while((pDirent = readdir(pDir)) != NULL){
		if(strcmp(pDirent->d_name,".") == 0 
			|| strcmp(pDirent->d_name,"..") == 0){
				continue;
		}

		if(pDirent->d_type == DT_DIR){
			memset(szBuf,0,256);
			snprintf(szBuf,256,"%s/%s",path.c_str(),pDirent->d_name);
			rmdirs(szBuf);
		}else{
			memset(szBuf,0,256);
			snprintf(szBuf,256,"%s/%s",path.c_str(),pDirent->d_name);
			remove(szBuf);//delete file
		}
	}

	closedir(pDir);

	if(0 != remove(path.c_str())){
		return false;
	}

	return true;
}
std::string get_version(){
    return std::string(MAIN_VERSION_STRING)+"."+std::string(MINOR_VERSION_STRING);
}


bool isPortUsed(int nPort){
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(socketfd ==-1){
		return true;	//套接字创建失败?基本不可能
	}
	if(nPort <=0){
		return true;		//true 表示不能使用该端口
	}
	struct sockaddr_in socketAddr;
	socketAddr.sin_family=AF_INET;
	socketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socketAddr.sin_port = htons(nPort);
	int nResult = bind(socketfd,(struct sockaddr*)&socketAddr,sizeof(socketAddr));
	if( nResult != -1){	//bind success port not be used
		close(socketfd);
		return false;
	}
	return true;
}
bool SyncEvent::timed_wait(unsigned int ms){
	MutexGuard g(m_mutex);
	if (!m_signaled) {
		struct timespec ts;
		struct timeval now;
		gettimeofday(&now,NULL);
		ts.tv_sec =now.tv_sec + ms/1000;
		ts.tv_nsec =0;
		int ret = pthread_cond_timedwait(&m_cond, m_mutex.mutex(), &ts);
		if (ret == ETIMEDOUT) return false;
	}
	if (!m_manual_reset) m_signaled = false;
	return true;
}
namespace str{
	std::vector<std::string> split(const std::string s,const std::string sep,int max){
		std::vector<std::string> v;
    	bool white_separ = (sep[0] == ' ' || sep[0] == '\t');

    	::size_t pos = 0, from = 0;
    	if (!s.empty() && sep[0] != '\n' && s[0] == sep[0]){
			from = 1;
    	}
	    while ((pos = s.find(sep, from)) != std::string::npos) {
	        if (!white_separ || pos != from) {
	            v.push_back(s.substr(from, pos - from));
	            if (v.size() == max){
					return v;
	        	}
			}
	        from = pos + 1;
	    }

    	if (from < s.size()){
			v.push_back(s.substr(from));
		}
    	return v;
	}
};

