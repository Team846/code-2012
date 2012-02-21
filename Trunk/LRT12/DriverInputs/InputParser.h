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
	
	//button assignments shorted by button number
	
	//Driver stick
	static const int LOWER_COLLECTOR = 1;
	static const int APPLY_CONFIG   = 5;
	static const int LOAD_CONFIG    = 6;
	static const int SAVE_CONFIG    = 7;
	static const int SHIFT 		    = 8;
	
	//Operator Stick
	static const int SHOOT		  		  = 1;
	static const int COLLECT_BALLS        = 2;
	static const int LOW_SPEED        	  = 2;
	static const int HIGH_SPEED           = 2;
	static const int TRAJECTORY_UP	 	  = 5;
	static const int TRAJECTORY_DOWN	  = 6;
	static const int DECREMENT_SPEED      = 7;
	static const int INCREMENT_SPEED 	  = 8;
	static const int WEDGE_DOWN		  	  = 9;
	static const int WEDGE_UP		  	  = 10;
	
};

#endif //_INTPUT_PARSER_H_
