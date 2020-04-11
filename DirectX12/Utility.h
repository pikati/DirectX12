#pragma once
#include <Stdio.h>
#ifdef _DEBUG
#   define MyOutputDebugString( str, ... ) \
      { \
        TCHAR c[256]; \
        sprintf( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#    define MyOutputDebugString( str, ... ) // ‹óŽÀ‘•
#endif

//ex.	MyOutputDebugString(("Vector( %f, %f, %f, %f )\n"), m_u, m_v, m_tw, m_th);
