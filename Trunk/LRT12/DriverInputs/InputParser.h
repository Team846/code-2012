#ifndef _INTPUT_PARSER_H_
#define _INTPUT_PARSER_H_

class ActionData;
class DebouncedJoystick;

class InputParser
{
public:
	InputParser();
	void ProcessInputs();
private:
	ActionData *m_action_ptr;
	DebouncedJoystick * m_driver_stick;
	DebouncedJoystick * m_operator_stick;
	DebouncedJoystick * m_driver_steering_wheel;
	int m_use_steering_wheel;
	
	//button assignments shorted by button number
	
	//Driver stick
#define DUMMY_BUTTON 11
	static const int LOWER_COLLECTOR = 1;
//	static const int AUTO_AIM_DIR   = 2;
	static const int AUTO_AIM_DIR   = DUMMY_BUTTON; //FIXME anurag
	static const int WHEEL_TOGGLE = 2; 

	static const int KEYTRACK 		= 3;
	static const int AUTOAIM    	= 4;
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
	static const int FENDER_SHOT_SELECT	  = 5;
	static const int KEY_SHOT_SELECT	  = 6;
	static const int DECREMENT_SPEED      = 7;
	static const int INCREMENT_SPEED 	  = 8;
	static const int WEDGE_DOWN		  	  = 9;
	static const int WEDGE_UP		  	  = 10;
	static const int PURGE		  	  	  = 11;
	
};

#endif //_INTPUT_PARSER_H_
