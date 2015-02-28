
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef SEVEN_SEG_H
#define SEVEN_SEG_H

// Seven Seg Enumeration of characters
enum sevenSeg { 
//	_           _     _           _     _     _     _     _     _           _           _     _       
// | |     |    _|    _|   |_|   |_    |_      |   |_|   |_|   |_|   |_    |      _|   |_    |_       
// |_|     |   |_     _|     |    _|   |_|     |   |_|    _|   | |   |_|   |_    |_|   |_    |        .
   SS_0, SS_1, SS_2, SS_3, SS_4, SS_5, SS_6, SS_7, SS_8, SS_9, SS_A, SS_B, SS_C, SS_D, SS_E, SS_F, SS_DP,
//  _                                   _     _           _     _                                         _                 _
// |     |_|   |_    |       |   |      _    | |    _    |_|   |      _    |_    | |         | |   |_|    _    |_|    _|    _|
// |_|   | |   |_|   |     |_|   |_    | |   | |   | |   |     |     |     |_    |_|   |_|    _           _     _|   |     | | 
   SS_G, SS_H, SS_h, SS_I, SS_J, SS_L, SS_m, SS_N, SS_n, SS_P, SS_R, SS_r, SS_t, SS_U, SS_u, SS_V, SS_v, SS_X, SS_Y, SS_Z, SS_lambda, 
//                                                    _                                
// |_  	      _|       | |       | |                           |_        |           |  
// |           |       | |                 | |        _          |         |       |   
   SS_char1, SS_char2, SS_char3, SS_char4, SS_char5, SS_char6, SS_char7, SS_char8, SS_char9,
//  _                                _
// |_  	       _          _          _|
//            |_          _|            
   SS_char10, SS_char11, SS_char12, SS_char13 };
//  
//  _
// 
// Default
	
////////////////////////////////////////////////////////////////////////////////
//Functionality - Writes output to a 7-segment display
//Parameter: An unsigned character to output
//Returns: None
void Write7Seg(unsigned char c) {
	
	// Using the following table:
		//		_A_
		//	F |		| B 
		//		_G_
		//	E |		| C
		//		_D_		. DP
	
	// Match your port and pin numbers:
	
	static unsigned char *sevenSeg_Port			= &PORTC;	// Seven segment port
	static const unsigned char sevenSeg_A		= 2; 		// Pin for segment A
	static const unsigned char sevenSeg_B		= 3; 		// Pin for segment B
	static const unsigned char sevenSeg_C		= 6; 		// Pin for segment C
	static const unsigned char sevenSeg_D		= 5; 		// Pin for segment D
	static const unsigned char sevenSeg_E		= 4; 		// Pin for segment E
	static const unsigned char sevenSeg_F		= 1; 		// Pin for segment F
	static const unsigned char sevenSeg_G		= 0; 		// Pin for segment G
	static const unsigned char sevenSeg_DP		= 7; 		// Pin for segment DP
	
	// Turns on all segments
	// *sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
	//					(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) |	(1<<sevenSeg_DP);	
	
	// Seven Seg character defintions
	switch (c) {
		case SS_0:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F)	;
			break;
		case SS_1:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) ;
			break;
		case SS_2:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_D) |	
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_3:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_G) ;
			break;
		case SS_4:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C)	|	
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_5:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_6:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_7:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) ;
			break;
		case SS_8:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_9:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_A:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_B:
			*sevenSeg_Port =	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_C:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_D:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_E:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_F:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_DP:
			*sevenSeg_Port =	(1<<sevenSeg_DP);
			break;
		case SS_G:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_H:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_h:
			*sevenSeg_Port =	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_I:
			*sevenSeg_Port =	(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_J:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	;
			break;
		case SS_L:
			*sevenSeg_Port =	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_m:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_N:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_n:
			*sevenSeg_Port =	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_P:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_R:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_r:
			*sevenSeg_Port =	(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_t:
			*sevenSeg_Port =	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_U:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_u:
			*sevenSeg_Port =	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	;
			break;
		case SS_V:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_F) ;
			break;
		case SS_v:
			*sevenSeg_Port =	(1<<sevenSeg_B) |
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_X:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_D) |
								(1<<sevenSeg_G) ;
			break;
		case SS_Y:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_Z:
			*sevenSeg_Port =	(1<<sevenSeg_B) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_lambda:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;
		case SS_char1:
			*sevenSeg_Port =	(1<<sevenSeg_E)	|	(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_char2:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |
								(1<<sevenSeg_G) ;
			break;
		case SS_char3:
			*sevenSeg_Port =	(1<<sevenSeg_B) |	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_F) ;
			break;
		case SS_char4:
			*sevenSeg_Port =	(1<<sevenSeg_B) |
								(1<<sevenSeg_F) ;
			break;
		case SS_char5:
			*sevenSeg_Port =	(1<<sevenSeg_C) |
								(1<<sevenSeg_E)	;
			break;
		case SS_char6:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_D) ;
			break;
		case SS_char7:
			*sevenSeg_Port =	(1<<sevenSeg_C) |
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;
		case SS_char8:
			*sevenSeg_Port =	(1<<sevenSeg_C) |
								(1<<sevenSeg_F) ;
			break;	
		case SS_char9:
			*sevenSeg_Port =	(1<<sevenSeg_B) |
								(1<<sevenSeg_E)	;	
			break;	
		case SS_char10:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|
								(1<<sevenSeg_F) |	(1<<sevenSeg_G) ;
			break;	
		case SS_char11:
			*sevenSeg_Port =	(1<<sevenSeg_D) |
								(1<<sevenSeg_E)	|	(1<<sevenSeg_G) ;
			break;	
		case SS_char12:
			*sevenSeg_Port =	(1<<sevenSeg_C) |	(1<<sevenSeg_D) |
								(1<<sevenSeg_G) ;
			break;	
		case SS_char13:
			*sevenSeg_Port =	(1<<sevenSeg_A)	|	(1<<sevenSeg_B) |	
								(1<<sevenSeg_G) ;
			break;	
	
		default:
			*sevenSeg_Port =	(1<<sevenSeg_G) ;
		break;
	}
}

#endif // SEVEN_SEG_H