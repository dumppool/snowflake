
class FCommandBindingSample
{
public:
	FCommandBindingSample();
	~FCommandBindingSample();

private:
};

class ICB
{
public:
	virtual void Method1(const string& input) = 0;
	virtual void Method2(const string& input) = 0;
};

class FCBParent : public ICB
{
public:
	explicit FCBParent(const string& name);

	virtual void Method1(const string& input) override;
	virtual void Method2(const string& input) override;

private:
	string Name;
};

class FCBChild : public FCBParent
{
public:
	explicit FCBChild(const string& name);

	virtual void Method1(const string& input) override;

private:
	string Name;
};

class FCBObjectRequest
{
public:
	FCBObjectRequest();
	~FCBObjectRequest();

	void Request1(const string& input);
	void Request2(const string& input);
	void Request3(const string& input);

private:
	int32 Id;
};

// 只在同一个线程活动的API操作对象
class FCBObject
{
public:
	FCBObject();
	~FCBObject();

	void Response1(const string& input);
	void Response2(const string& input);
	void Response3(const string& input);
};

class FCBObjectAllocator
{
public:
	static FCBObjectAllocator* Get()
	{
		static FCBObjectAllocator SInst;
		return &SInst;
	}

	FCBObjectAllocator();
	~FCBObjectAllocator();

	int32 RequestAlloc();
	void RequestDealloc(int32 id);

	// 在API操作对象的活动线程loop最初分配
	void FlushAllocating();

	// 在API操作对象的活动线程loop最后回收
	void FlushDeallocating();

	FCBObject* GetObj(int32 id);

private:
	map<int32, FCBObject*> AllocatedObjects;
	vector<int32> Allocating;
	vector<int32> Deallocating;

	vector<int32> UnusedIds;
	atomic<int32> LastAllocatedId;
	static const int32 SMaxAllocId = (1 << 30);
};

static void PushCommand(const function<void()>& cmd)
{

}