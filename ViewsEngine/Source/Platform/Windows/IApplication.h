#pragma once

#define ENTRYAPP(x) Win32::IApplication* EntryApplication() { return new x; }

namespace Win32
{
	class IApplication
	{

	public:

		IApplication();

		virtual ~IApplication() {};

	public:

		/* Called to set up our per game settings */
		virtual void SetupPerGameSettings() = 0;

		/* Initialize the application */
		virtual void Initialize() = 0;

		virtual void Shutdown() = 0;

	protected:

		/* Game Loop */
		virtual void Update(const float dt) = 0;

	};

	IApplication* EntryApplication();

}
