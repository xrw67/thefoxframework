

class EventLoop
{
public:
	void exec();
public:
	std
};


EventLoop::exec()
{
	Eventable *recever = NULL;
	Event *e = NULL;
	while (getEvent(recever, e)) {
		if (!recever && !e)
			break;
		recever->event(e);
	}
}