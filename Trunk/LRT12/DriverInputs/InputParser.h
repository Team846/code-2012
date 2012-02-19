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
	ActionData *m_action;
	DebouncedJoystick * m_driver_stick;
	DebouncedJoystick * m_operator_stick;
	
	//button assignments shorted by button number
	
	//Driver stick
	static const int APPLY_CONFIG = 5;
	static const int LOAD_CONFIG  = 6;
	static const int SAVE_CONFIG  = 7;
	static const int SHIFT 		  = 8;
	
	//Operator Stick
	static const int SHOOT		  = 2;
	static const int DECREMENT_BALL_COUNT = 3;
	static const int INCREMENT_BALL_COUNT = 3;
	
};

#endif //_INTPUT_PARSER_H_
