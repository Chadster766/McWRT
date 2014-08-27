/*
 *
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *
 *  As modified by Rene van Paassen, May 19, 2005
 *  Event device test program
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Ucitelska 1576, Prague 8, 182 00 Czech Republic
 */

#include <linux/input.h>
#include <linux/version.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

char *events[EV_MAX + 1] = { "Reset", "Key", "Relative", "Absolute", "MSC", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, "LED", "Sound", NULL, "Repeat", "ForceFeedback", "Power", "ForceFeedbackStatus"};

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 0)

/* Keys updated with version 2.6.11, am assuming this is 2.6 style */
char *keys[KEY_MAX + 1] = 
  { "Reserved", "Esc", /* 0, 1 */
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", /* 2 - 11 */
    "Minus", "Equal", "Backspace", "Tab", /* 12 - 15 */
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", /* 16 - 25 */
    "LeftBrace", "RightBrace", "Enter", "LeftControl", /* 26 - 29 */
    "A", "S", "D", "F", "G", "H", "J", "K", "L", /* 30 - 38 */
    "Semicolon", "Apostrophe", "Grave", "LeftShift", "BackSlash", /* 39 - 43 */
    "Z", "X", "C", "V", "B", "N", "M",  /* 44 - 50 */
    "Comma", "Dot", "Slash", "RightShift", "KPAsterisk", 
    "LeftAlt", "Space", "CapsLock",  /* 51 - 58 */
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", /* 59 - 68 */
    "NumLock", "ScrollLock", /* 69, 70 */
    "KP7", "KP8", "KP9", "KPMinus", /* 71 - 74 */
    "KP4", "KP5", "KP6", "KPPlus", /* 75 - 78 */
    "KP1", "KP2", "KP3", "KP0", "KPDot", /* 79 - 83 */
    NULL, 
    "ZENKAKUHANKAKU", /* 85 */
    "102nd", "F11", "F12", /* 86 - 88 */
    "R0", "KATAKANA", "HIRAGANA", "HENKAN", /* 89 - 92 */
    "KATAKANAHIRAGANA", "MUHENKAN", "KPJPCOMMA", /* 93 - 95 */
    "KPEnter", "RightCtrl", "KPSlash", "SysRq", /* 96 - 99 */
    "RightAlt", "LineFeed", /*100 - 101 */
    "Home", "Up", "PageUp", "Left", "Right", "End", "Down", 
    "PageDown", "Insert", "Delete", /* 102 - 111 */
    "Macro", "Mute", "VolumeDown", "VolumeUp", "Power", /* 112 - 116 */
    "KPEqual", "KPPlusMinus", "Pause", /* 117 - 119 */
    NULL, 
    "KPComma", /* 121 */
    "Hanguel", "Hanja", "Yen", /* 122 - 124 */
    "LeftMeta", "RightMeta", "Compose", /* 125 - 127 */
    "Stop", "Again", "Props", "Undo", "Front", "Copy", "Open", 
    "Paste", "Find", "Cut", "Help", "Menu", "Calc", "Setup",
    "Sleep", "WakeUp", "File", "SendFile", "DeleteFile", "X-fer", 
    "Prog1", "Prog2", "WWW", "MSDOS", "Coffee", "Direction",
    "CycleWindows", "Mail", "Bookmarks", "Computer", 
    "Back", "Forward", "CloseCD", "EjectCD", "EjectCloseCD", 
    "NextSong", "PlayPause", "PreviousSong", "StopCD", 
    "Record", "Rewind", "Phone", /* 128 - 169 */
    "ISOKey", "Config", "HomePage", "Refresh", "Exit", 
    "Move", "Edit", "ScrollUp", "ScrollDown", /* 170 - 178 */
    "KPLeftParenthesis", "KPRightParenthesis", /* 179 - 180 */
    NULL, NULL,
    "F13", "F14", "F15", "F16", "F17", "F18", 
    "F19", "F20", "F21", "F22", "F23", "F24", /* 183 - 194 */
    NULL, NULL, NULL, NULL, NULL,
    "PlayCD", "PauseCD", "Prog3", "Prog4", NULL, 
    "Suspend", "Close", /* 200 - 206 */
    "Play", "FastForward", "BassBoost", "Print", "HP", "Camera", 
    "Sound", "Question", "Email", "Chat", "Search", 
    "Connect", "Finance", "Sport", "Shop", "AltErase", 
    "Cancel", "BrightnessDown", "BrightnessUp", "Media", /* 207 - 226 */
    "SwitchVideoMode", 
    "KBDIllumToggle", "KBDIllumDown", "KBDIllumUp", /* 227 - 230 */
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "Unknown", /* 240 */ 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "Btn0", "Btn1", "Btn2", "Btn3", "Btn4", 
    "Btn5", "Btn6", "Btn7", "Btn8", "Btn9", /* 256 - 265 */
    NULL, NULL, NULL, NULL, NULL, NULL,
    "LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", 
    "ExtraBtn", "ForwardBtn", "BackBtn", "TaskBtn", /* 0x110 - 0x117 */
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", 
    "TopBtn2", "PinkieBtn", "BaseBtn", "BaseBtn2", 
    "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6",
    NULL, NULL, NULL, "BtnDead", /* 0x120 - 0x12f */
    "BtnA", "BtnB", "BtnC", "BtnX", 
    "BtnY", "BtnZ", "BtnTL", "BtnTR", 
    "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", 
    "BtnMode", "BtnThumbL", "BtnThumbR", /* 0x130 - 0x13e */ NULL,
    "ToolPen", "ToolRubber", "ToolBrush", "ToolPencil", 
    "ToolAirbrush", "ToolFinger", "ToolMouse", "ToolLens", /* 0x140-0x147 */
    NULL, NULL, "Touch", "Stylus", 
    "Stylus2", "DoubleTap", "TripleTap", /* 0x14a - 0x14e */ NULL,
    "GearUp", "GearDown", /* 0x150, 0x151 */
    NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "Ok", "Select", "Goto", "Clear", "Power2", "Option", "Info", "Time",
    "Vendor", "Archive", "Program", "Channel",
    "Favorites", "Epg", "Pvr", "Mhp", 
    "Language", "Title", "Subtitle", "Angle",
    "Zoom", "Mode", "Keyboard", "Screen",
    "Pc", "Tv", "Tv2", "Vcr", "Vcr2", "Sat", "Sat2", "Cd",
    "Tape", "Radio", "Tuner", "Player", "Text", "Dvd", "Aux", "Mp3",
    "Audio", "Video", "Directory", "List",
    "Memo", "Calendar", "Red", "Green", 
    "Yellow", "Blue", "Channelup", "Channeldown",
    "First", "Last", "Ab", "Next", 
    "Restart", "Slow", "Shuffle", "Break", 
    "Previous", "Digits", "Teen", "Twen", /* 160 - 19f */
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "DelEol", "DelEos", "InsLine", "DelLine", /* 1c0 - 1c3 */
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "FN", "FN_ESC", 
    "FN_F1", "FN_F2", "FN_F3", "FN_F4", "FN_F5", 
    "FN_F6", "FN_F7", "FN_F8", "FN_F9", "FN_F10", 
    "FN_F11", "FN_F12", 
    "FN_1", "FN_2", "FN_D", "FN_E", "FN_F", "FN_S", "FN_B" /* 0x1d0 - 0x1e4 */
  };

#else

/* Keys I used with 2.4 kernels. */
char *keys[KEY_MAX + 1] = { "Reserved", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "Minus", "Equal", "Backspace",
"Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "LeftBrace", "RightBrace", "Enter", "LeftControl", "A", "S", "D", "F", "G",
"H", "J", "K", "L", "Semicolon", "Apostrophe", "Grave", "LeftShift", "BackSlash", "Z", "X", "C", "V", "B", "N", "M", "Comma", "Dot",
"Slash", "RightShift", "KPAsterisk", "LeftAlt", "Space", "CapsLock", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
"NumLock", "ScrollLock", "KP7", "KP8", "KP9", "KPMinus", "KP4", "KP5", "KP6", "KPPlus", "KP1", "KP2", "KP3", "KP0", "KPDot", "103rd",
"F13", "102nd", "F11", "F12", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "KPEnter", "RightCtrl", "KPSlash", "SysRq",
"RightAlt", "LineFeed", "Home", "Up", "PageUp", "Left", "Right", "End", "Down", "PageDown", "Insert", "Delete", "Macro", "Mute",
"VolumeDown", "VolumeUp", "Power", "KPEqual", "KPPlusMinus", "Pause", "F21", "F22", "F23", "F24", "KPComma", "LeftMeta", "RightMeta",
"Compose", "Stop", "Again", "Props", "Undo", "Front", "Copy", "Open", "Paste", "Find", "Cut", "Help", "Menu", "Calc", "Setup",
"Sleep", "WakeUp", "File", "SendFile", "DeleteFile", "X-fer", "Prog1", "Prog2", "WWW", "MSDOS", "Coffee", "Direction",
"CycleWindows", "Mail", "Bookmarks", "Computer", "Back", "Forward", "CloseCD", "EjectCD", "EjectCloseCD", "NextSong", "PlayPause",
"PreviousSong", "StopCD", "Record", "Rewind", "Phone", "ISOKey", "Config", "HomePage", "Refresh", "Exit", "Move", "Edit", "ScrollUp",
"ScrollDown", "KPLeftParenthesis", "KPRightParenthesis",
"International1", "International2", "International3", "International4", "International5",
"International6", "International7", "International8", "International9",
"Language1", "Language2", "Language3", "Language4", "Language5", "Language6", "Language7", "Language8", "Language9",
NULL, 
"PlayCD", "PauseCD", "Prog3", "Prog4", "Suspend", "Close",
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
"Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9",
NULL, NULL,  NULL, NULL, NULL, NULL,
"LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", "ExtraBtn", "ForwardBtn", "BackBtn",
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
"Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn",
"BaseBtn", "BaseBtn2", "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6",
NULL, NULL, NULL, "BtnDead",
"BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode",
"BtnThumbL", "BtnThumbR", NULL,
"ToolPen", "ToolRubber", "ToolBrush", "ToolPencil", "ToolAirbrush", "ToolFinger", "ToolMouse", "ToolLens", NULL, NULL,
"Touch", "Stylus", "Stylus2" };
#endif

char *absval[5] = { "Value", "Min  ", "Max  ", "Fuzz ", "Flat " };
char *relatives[REL_MAX + 1] = 
  { "X", "Y", "Z", NULL, 
    NULL, NULL, "HWheel", "Dial", 
    /* 0x08: */ "Wheel", "MISC" };
char *absolutes[ABS_MAX + 1] = 
  { "X", "Y", "Z", "Rx",  "Ry", "Rz", "Throttle", "Rudder", 
    "Wheel", "Gas", "Brake", NULL, NULL, NULL, NULL, NULL,
    /* 0x10: */ "Hat0X", "Hat0Y", "Hat1X", "Hat1Y", 
    "Hat2X", "Hat2Y", "Hat3X", "Hat 3Y", 
    "Pressure", "Distance", "XTilt", "YTilt", 
    "ToolWidth", NULL, NULL, NULL, 
    /* 0x20: */ "Volume", NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, 
    /* 0x28: */ "Misc" };
char *leds[LED_MAX + 1] = { "NumLock", "CapsLock", "ScrollLock", "Compose", "Kana", "Sleep", "Suspend", "Mute" };
char *repeats[REP_MAX + 1] = { "Delay", "Period" };
char *sounds[SND_MAX + 1] = { "Bell", "Click" };

char **names[EV_MAX + 1] = { events, keys, relatives, absolutes, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, leds, sounds, NULL, repeats, NULL, NULL, NULL };

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

int main (int argc, char **argv)
{
	int fd, rd, i, j, k;
	struct input_event ev[64];
	int version;
	unsigned short id[4];
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	char name[256] = "Unknown";
	int abs[5];

#if 0
	for (i = 0; i < KEY_MAX; i++) {
	  printf("Key %d, name \"%s\"\n", i, keys[i] ? keys[i]: "?");
	}
#endif
	    
	if (argc < 2) {
		printf ("Usage: evtest /dev/inputX\n");
		printf ("Where X = input device number\n");
		exit (1);
	}

	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("evtest");
		exit(1);
	}

	ioctl(fd, EVIOCGVERSION, &version);
	printf("Input driver version is %d.%d.%d\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);

	ioctl(fd, EVIOCGID, id);
	printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
		id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);

	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	printf("Input device name: \"%s\"\n", name);

	memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
	printf("Supported events:\n");

	for (i = 0; i < EV_MAX; i++)
    {
		if (test_bit(i, bit[0])) 
        {
			printf("  Event type %d (%s)\n", i, events[i] ? events[i] : "?");
			ioctl(fd, EVIOCGBIT(i, KEY_MAX), bit[i]);
			for (j = 0; j < KEY_MAX; j++) 
            {
				if (test_bit(j, bit[i])) 
                {
					printf("    Event code %d (%s)\n", j, names[i] ? (names[i][j] ? names[i][j] : "?") : "?");
					if (i == EV_ABS) 
                    {
						ioctl(fd, EVIOCGABS(j), abs);
						for (k = 0; k < 5; k++)
                        {
							if ((k < 3) || abs[k])
                            {
								printf("      %s %6d\n", absval[k], abs[k]);
                            }
                        }
					}
				}
            }
		}
    }	

	printf("Testing ... (interrupt to exit)\n");

	while (1) 
    {
		rd = read(fd, ev, sizeof(struct input_event) * 64);

		if (rd < (int) sizeof(struct input_event)) 
        {
			printf("yyy\n");
			perror("\nevtest: error reading");
			exit (1);
		}

		for (i = 0; i < rd / sizeof(struct input_event); i++)
        {
			printf("Event: time %ld.%06ld, type %d (%s), code %d (%s), value %d\n",
				ev[i].time.tv_sec, 
                ev[i].time.tv_usec, 
                ev[i].type,
				events[ev[i].type] ? events[ev[i].type] : "?",
				ev[i].code,
				names[ev[i].type] ? (names[ev[i].type][ev[i].code] ? names[ev[i].type][ev[i].code] : "?") : "?",
				ev[i].value);
        }
	}
}
