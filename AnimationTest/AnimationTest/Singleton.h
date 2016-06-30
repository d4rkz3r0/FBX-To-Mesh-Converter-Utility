#pragma once

template<typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if(!gInstance)
		{
			gInstance = new T();
		}
		return gInstance;
	}

	static void DestroyInstance()
	{
		delete gInstance;
		gInstance = nullptr;
	}

private:
	static T* gInstance;
};

//Static Variable Initalization
template <class T> T*  Singleton<T>::gInstance = nullptr;
