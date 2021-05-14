#ifndef __MINPUT_HPP__
#define __MINPUT_HPP__

#include "minput.h"

namespace minput
{
	class input_t
	{
	public:
		inline input_t()
		{
		}
		
		inline ~input_t()
		{
			destroy();
		}
	
		inline bool create( void* instance, void* window )
		{
			ptr = minput_device( instance, window );
			return ptr != nullptr;
		}
		
		inline void read( EDirection* direction )
		{
			minput_device_read( ptr, direction );
		}

		inline void readKeys(unsigned int* pKeys, unsigned int* pRet, unsigned int size) {
			minput_device_read_keys(ptr, pKeys, pRet, size);
		}
		
		inline void destroy()
		{
			if ( ptr != nullptr )
			{
				minput_device_destroy( ptr );
				ptr = nullptr;
			}
		}
		
	private:
	
		void* ptr;
	};
}


#endif