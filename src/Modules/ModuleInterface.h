class IModule
{
public:
	virtual ~IModule() {};
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Terminate() = 0;
};