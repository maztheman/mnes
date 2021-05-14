#ifndef __MINPUT_H__
#define __MINPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined _MSC_VER
#	if !defined DLL_EXPORT && defined MINPUT_STATIC
#		define DLL_EXPORT
#	endif
#	if defined DLL_EXPORT
#		define MINPUT_EXPORT __declspec(dllexport)
#	else
#		define MINPUT_EXPORT __declspec(dllimport)
#	endif
#endif


#define MI_ESCAPE          0x01
#define MI_1               0x02
#define MI_2               0x03
#define MI_3               0x04
#define MI_4               0x05
#define MI_5               0x06
#define MI_6               0x07
#define MI_7               0x08
#define MI_8               0x09
#define MI_9               0x0A
#define MI_0               0x0B
#define MI_MINUS           0x0C    /* - on main keyboard */
#define MI_EQUALS          0x0D
#define MI_BACK            0x0E    /* backspace */
#define MI_TAB             0x0F
#define MI_Q               0x10
#define MI_W               0x11
#define MI_E               0x12
#define MI_R               0x13
#define MI_T               0x14
#define MI_Y               0x15
#define MI_U               0x16
#define MI_I               0x17
#define MI_O               0x18
#define MI_P               0x19
#define MI_LBRACKET        0x1A
#define MI_RBRACKET        0x1B
#define MI_RETURN          0x1C    /* Enter on main keyboard */
#define MI_LCONTROL        0x1D
#define MI_A               0x1E
#define MI_S               0x1F
#define MI_D               0x20
#define MI_F               0x21
#define MI_G               0x22
#define MI_H               0x23
#define MI_J               0x24
#define MI_K               0x25
#define MI_L               0x26
#define MI_SEMICOLON       0x27
#define MI_APOSTROPHE      0x28
#define MI_GRAVE           0x29    /* accent grave */
#define MI_LSHIFT          0x2A
#define MI_BACKSLASH       0x2B
#define MI_Z               0x2C
#define MI_X               0x2D
#define MI_C               0x2E
#define MI_V               0x2F
#define MI_B               0x30
#define MI_N               0x31
#define MI_M               0x32
#define MI_COMMA           0x33
#define MI_PERIOD          0x34    /* . on main keyboard */
#define MI_SLASH           0x35    /* / on main keyboard */
#define MI_RSHIFT          0x36
#define MI_MULTIPLY        0x37    /* * on numeric keypad */
#define MI_LMENU           0x38    /* left Alt */
#define MI_SPACE           0x39
#define MI_CAPITAL         0x3A
#define MI_F1              0x3B
#define MI_F2              0x3C
#define MI_F3              0x3D
#define MI_F4              0x3E
#define MI_F5              0x3F
#define MI_F6              0x40
#define MI_F7              0x41
#define MI_F8              0x42
#define MI_F9              0x43
#define MI_F10             0x44
#define MI_NUMLOCK         0x45
#define MI_SCROLL          0x46    /* Scroll Lock */
#define MI_NUMPAD7         0x47
#define MI_NUMPAD8         0x48
#define MI_NUMPAD9         0x49
#define MI_SUBTRACT        0x4A    /* - on numeric keypad */
#define MI_NUMPAD4         0x4B
#define MI_NUMPAD5         0x4C
#define MI_NUMPAD6         0x4D
#define MI_ADD             0x4E    /* + on numeric keypad */
#define MI_NUMPAD1         0x4F
#define MI_NUMPAD2         0x50
#define MI_NUMPAD3         0x51
#define MI_NUMPAD0         0x52
#define MI_DECIMAL         0x53    /* . on numeric keypad */
#define MI_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define MI_F11             0x57
#define MI_F12             0x58
#define MI_F13             0x64    /*                     (NEC PC98) */
#define MI_F14             0x65    /*                     (NEC PC98) */
#define MI_F15             0x66    /*                     (NEC PC98) */
#define MI_KANA            0x70    /* (Japanese keyboard)            */
#define MI_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define MI_CONVERT         0x79    /* (Japanese keyboard)            */
#define MI_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define MI_YEN             0x7D    /* (Japanese keyboard)            */
#define MI_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define MI_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define MI_PREVTRACK       0x90    /* Previous Track (MI_CIRCUMFLEX on Japanese keyboard) */
#define MI_AT              0x91    /*                     (NEC PC98) */
#define MI_COLON           0x92    /*                     (NEC PC98) */
#define MI_UNDERLINE       0x93    /*                     (NEC PC98) */
#define MI_KANJI           0x94    /* (Japanese keyboard)            */
#define MI_STOP            0x95    /*                     (NEC PC98) */
#define MI_AX              0x96    /*                     (Japan AX) */
#define MI_UNLABELED       0x97    /*                        (J3100) */
#define MI_NEXTTRACK       0x99    /* Next Track */
#define MI_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define MI_RCONTROL        0x9D
#define MI_MUTE            0xA0    /* Mute */
#define MI_CALCULATOR      0xA1    /* Calculator */
#define MI_PLAYPAUSE       0xA2    /* Play / Pause */
#define MI_MEDIASTOP       0xA4    /* Media Stop */
#define MI_VOLUMEDOWN      0xAE    /* Volume - */
#define MI_VOLUMEUP        0xB0    /* Volume + */
#define MI_WEBHOME         0xB2    /* Web home */
#define MI_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define MI_DIVIDE          0xB5    /* / on numeric keypad */
#define MI_SYSRQ           0xB7
#define MI_RMENU           0xB8    /* right Alt */
#define MI_PAUSE           0xC5    /* Pause */
#define MI_HOME            0xC7    /* Home on arrow keypad */
#define MI_UP              0xC8    /* UpArrow on arrow keypad */
#define MI_PRIOR           0xC9    /* PgUp on arrow keypad */
#define MI_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define MI_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define MI_END             0xCF    /* End on arrow keypad */
#define MI_DOWN            0xD0    /* DownArrow on arrow keypad */
#define MI_NEXT            0xD1    /* PgDn on arrow keypad */
#define MI_INSERT          0xD2    /* Insert on arrow keypad */
#define MI_DELETE          0xD3    /* Delete on arrow keypad */
#define MI_LWIN            0xDB    /* Left Windows key */
#define MI_RWIN            0xDC    /* Right Windows key */
#define MI_APPS            0xDD    /* AppMenu key */
#define MI_POWER           0xDE    /* System Power */
#define MI_SLEEP           0xDF    /* System Sleep */
#define MI_WAKE            0xE3    /* System Wake */
#define MI_WEBSEARCH       0xE5    /* Web Search */
#define MI_WEBFAVORITES    0xE6    /* Web Favorites */
#define MI_WEBREFRESH      0xE7    /* Web Refresh */
#define MI_WEBSTOP         0xE8    /* Web Stop */
#define MI_WEBFORWARD      0xE9    /* Web Forward */
#define MI_WEBBACK         0xEA    /* Web Back */
#define MI_MYCOMPUTER      0xEB    /* My Computer */
#define MI_MAIL            0xEC    /* Mail */
#define MI_MEDIASELECT     0xED    /* Media Select */

	/*
	*  Alternate names for keys, to facilitate transition from DOS.
	*/
#define MI_BACKSPACE       MI_BACK            /* backspace */
#define MI_NUMPADSTAR      MI_MULTIPLY        /* * on numeric keypad */
#define MI_LALT            MI_LMENU           /* left Alt */
#define MI_CAPSLOCK        MI_CAPITAL         /* CapsLock */
#define MI_NUMPADMINUS     MI_SUBTRACT        /* - on numeric keypad */
#define MI_NUMPADPLUS      MI_ADD             /* + on numeric keypad */
#define MI_NUMPADPERIOD    MI_DECIMAL         /* . on numeric keypad */
#define MI_NUMPADSLASH     MI_DIVIDE          /* / on numeric keypad */
#define MI_RALT            MI_RMENU           /* right Alt */
#define MI_UPARROW         MI_UP              /* UpArrow on arrow keypad */
#define MI_PGUP            MI_PRIOR           /* PgUp on arrow keypad */
#define MI_LEFTARROW       MI_LEFT            /* LeftArrow on arrow keypad */
#define MI_RIGHTARROW      MI_RIGHT           /* RightArrow on arrow keypad */
#define MI_DOWNARROW       MI_DOWN            /* DownArrow on arrow keypad */
#define MI_PGDN            MI_NEXT            /* PgDn on arrow keypad */




enum EDirection { eDIRECTION_IDLE = 0, eDIRECTION_LEFT = 1, eDIRECTION_RIGHT = 2, eDIRECTION_UP = 3, eDIRECTION_DOWN = 4 };

MINPUT_EXPORT void* minput_device(void* instance,void* window);
MINPUT_EXPORT void	minput_device_destroy( void* device );
MINPUT_EXPORT void 	minput_device_read(void* device, EDirection* direction);
MINPUT_EXPORT void 	minput_device_read_keys(void* device, unsigned int* pKeys, unsigned int* pRet, unsigned int size);


#ifdef __cplusplus
}
#endif

#endif