#ifndef _INTPUT_PARSER_H_
#define _INTPUT_PARSER_H_

#include "../flags.h"

class ActionData;
class DebouncedJoystick;

#if SKETCHY_SHIT_ENABLED
class NetworkHelper;
#endif

class InputParser
{
public:
	InputParser();
	void ProcessInputs();
private:
	ActionData *m_action_ptr;
	DebouncedJoystick * m_driver_stick;
	DebouncedJoystick * m_operator_stick;

#if SKETCHY_SHIT_ENABLED
	NetworkHelper *m_network_helper;
#endif
	//button assignments shorted by button number
	
	//Driver stick
	static const int LOWER_COLLECTOR = 1;
	static const int KEYTRACK 		= 3;
	static const int BRIDGEBALANCE	= 4;
	static const int APPLY_CONFIG   = 5;
	static const int LOAD_CONFIG    = 6;
	static const int SAVE_CONFIG    = 7;
	static const int SHIFT 		    = 8;
	static const int POSITION_HOLD	= 9;
	static const int RESET_ZERO		= 10;
	
	//Operator Stick
	static const int SHOOT		  		  = 1;
	static const int COLLECT_BALLS        = 2;
	static const int LOWER_SHOT        	  = 3;
	static const int HIGH_SPEED           = 4;
	static const int FENDER_SHOT_SELECT	 	  = 5;
	static const int KEY_SHOT_SELECT	  = 6;
	static const int DECREMENT_SPEED      = 7;
	static const int INCREMENT_SPEED 	  = 8;
	static const int WEDGE_DOWN		  	  = 9;
	static const int WEDGE_UP		  	  = 10;
	static const int PURGE		  	  	  = 11;
	
};

#endif //_INTPUT_PARSER_H_
