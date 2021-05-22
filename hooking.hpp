#pragma once

#define QWORD __int64

class Hooking
{
public:
	static Hooking& GetInstance()
	{
		static Hooking instance;
		return instance;
	}

	Hooking();

	void SetDiskFs(bool toggle) noexcept;
	bool IsDiskFs() const noexcept;

private:
	bool isDiskFs = false;
};

#define FS 0x14108BBC0 // g_pFS
#define DISKFS 0x14108BBC8 // g_pDiskFS
