#ifndef __searob_object_h__
#define __searob_object_h__

/*
 * Abstract class for any object with a lifestime that needs to to be updated/polled to 
 * move the state machine forward.
 */
class SeaRobObject {
	public:
  						SeaRobObject();
  				virtual ~SeaRobObject();
  		
  		virtual bool 	Init(unsigned long startTime) = 0;			
  		virtual void	ProcessLoop(unsigned long updateTime) = 0;
  			
	private:
  		static int				s_nextId;
  						
	protected:
		const int        		_objId;
		const unsigned long		_creationTime;
};

#endif // __searob_object_h__
