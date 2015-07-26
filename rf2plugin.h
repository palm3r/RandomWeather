#pragma once
#include "InternalsPlugin.hpp"
#include "ini.h"
#include "strformat.h"

#include <Windows.h>
#include <string>
#include <deque>
#include <boost/format.hpp>

namespace rf2 {

	class plugin : public InternalsPluginV07
	{
	public:
		plugin(std::string const& name)
			: name_(name), server_(false)
			, config_((boost::format(".\\plugins\\%1%\\%1%.ini") % name).str())
		{
		}

		virtual ~plugin()
		{
		}

		std::string const& Name() const { return name_; }
		bool isServer() const { return server_; }

		template <typename T>
		T Config(std::string const& section, std::string const& key, T const& defval = T())
		{
			return config_[section][key].as<T>(defval);
		}

		template <typename... Args>
		void DisplayMessage(std::string const& format, Args&&... args)
		{
			messages_.push_back(strformat(format, std::forward<Args>(args)...));
		}

	protected:
		virtual void OnStartup(long version) {}
		virtual void OnShutdown() {}
		virtual void OnLoad() {}
		virtual void OnUnload() {}
		virtual void OnStartSession() {}
		virtual void OnEndSession() {}
		virtual void OnEnterRealtime() {}
		virtual void OnExitRealtime() {}
		virtual bool OnWantsScoringUpdates() { return false; }
		virtual void OnUpdateScoring(const ScoringInfoV01 &info) {}
		virtual long OnWantsTelemetryUpdates() { return 0; }
		virtual void OnUpdateTelemetry(const TelemInfoV01 &info) {}
		virtual bool OnWantsGraphicsUpdates() { return false; }
		virtual void OnUpdateGraphics(const GraphicsInfoV01 &info) {}
		virtual bool OnRequestCommentary(CommentaryRequestInfoV01 &info) { return false; }
		virtual bool OnHasHardwareInputs() { return false; }
		virtual void OnUpdateHardware(const double fDT) {}
		virtual void OnEnableHardware() {}
		virtual void OnDisableHardware() {}
		virtual bool OnCheckHWControl(const char* const controlName, double &fRetVal) { return false; }
		virtual bool OnForceFeedback(double &forceValue) { return false; }
		virtual void OnError(const char* const msg) {}
		virtual void OnSetPhysicsOptions(PhysicsOptionsV01 &options) {}
		virtual unsigned char OnWantsToViewVehicle(CameraControlInfoV01 &camControl) { return 0; }
		virtual void OnUpdateGraphics(const GraphicsInfoV02 &info) {}
		virtual bool OnWantsToDisplayMessage(MessageInfoV01 &info) { return false; }
		virtual void OnSetEnvironment(const EnvironmentInfoV01 &info) {}
		virtual void OnInitScreen(const ScreenInfoV01 &info) {}
		virtual void OnUninitScreen(const ScreenInfoV01 &info) {}
		virtual void OnDeactivateScreen(const ScreenInfoV01 &info) {}
		virtual void OnReactivateScreen(const ScreenInfoV01 &info) {}
		virtual void OnRenderScreenBeforeOverlays(const ScreenInfoV01 &info) {}
		virtual void OnRenderScreenAfterOverlays(const ScreenInfoV01 &info) {}
		virtual void OnPreReset(const ScreenInfoV01 &info) {}
		virtual void OnPostReset(const ScreenInfoV01 &info) {}
		virtual bool OnInitCustomControl(CustomControlInfoV01 &info) { return false; }
		virtual bool OnWantsWeatherAccess() { return false; }
		virtual bool OnAccessWeather(double trackNodeSize, WeatherControlInfoV01 &info) { return false; }
		virtual void OnThreadStarted(long type) {}
		virtual void OnThreadStopping(long type) {}
		virtual bool OnGetCustomVariable(long i, CustomVariableV01 &var) { return false; }
		virtual void OnAccessCustomVariable(CustomVariableV01 &var) {}
		virtual void OnGetCustomVariableSetting(CustomVariableV01 &var, long i, CustomSettingV01 &setting) {}
		virtual bool OnWantsMultiSessionRulesAccess() { return false; }
		virtual bool OnAccessMultiSessionRules(MultiSessionRulesV01 &info) { return false; }
		virtual bool OnWantsTrackRulesAccess() { return false; }
		virtual bool OnAccessTrackRules(TrackRulesV01 &info) { return false; }
		virtual bool OnWantsPitMenuAccess() { return false; }
		virtual bool OnAccessPitMenu(PitMenuV01 &info) { return false; }

	private:
		std::string name_;
		bool server_;
		ini::file config_;
		std::deque<std::string> messages_;

		void Startup(long version)
		{
			char path[_MAX_PATH];
			GetModuleFileName(0, path, sizeof(path));
			server_ = _stricmp(path, "rFactor2 Dedicated.exe") == 0;

			OnStartup(version);
		}

		void Shutdown()
		{
			OnShutdown();
		}

		void Load()
		{
			OnLoad();
		}

		void Unload()
		{
			OnUnload();
		}

		void StartSession()
		{
			OnStartSession();
		}

		void EndSession()
		{
			OnEndSession();
		}

		void EnterRealtime()
		{
			OnEnterRealtime();
		}

		void ExitRealtime()
		{
			OnExitRealtime();
		}

		bool WantsScoringUpdates()
		{
			return OnWantsScoringUpdates();
		}

		void UpdateScoring(const ScoringInfoV01 &info)
		{
			OnUpdateScoring(info);
		}

		long WantsTelemetryUpdates()
		{
			return OnWantsTelemetryUpdates();
		}

		void UpdateTelemetry(const TelemInfoV01 &info)
		{
			OnUpdateTelemetry(info);
		}

		bool WantsGraphicsUpdates()
		{
			return OnWantsGraphicsUpdates();
		}

		void UpdateGraphics(GraphicsInfoV01 const& info)
		{
			OnUpdateGraphics(info);
		}

		bool RequestCommentary(CommentaryRequestInfoV01 &info)
		{
			return OnRequestCommentary(info);
		}

		bool HasHardwareInputs()
		{
			return OnHasHardwareInputs();
		}

		void UpdateHardware(const double fDT)
		{
			OnUpdateHardware(fDT);
		}

		void EnableHardware()
		{
			OnEnableHardware();
		}

		void DisableHardware()
		{
			OnDisableHardware();
		}

		bool CheckHWControl(const char* const controlName, double &fRetVal)
		{
			return OnCheckHWControl(controlName, fRetVal);
		}

		bool ForceFeedback(double &forceValue)
		{
			return OnForceFeedback(forceValue);
		}

		void Error(const char* const msg)
		{
			OnError(msg);
		}

		void SetPhysicsOptions(PhysicsOptionsV01 &options)
		{
			OnSetPhysicsOptions(options);
		}

		unsigned char WantsToViewVehicle(CameraControlInfoV01 &camControl)
		{
			return OnWantsToViewVehicle(camControl);;
		}

		void UpdateGraphics(const GraphicsInfoV02 &info)
		{
			OnUpdateGraphics(info);
		}
		
		bool WantsToDisplayMessage(MessageInfoV01 &info)
		{
			if (!messages_.empty())
			{
				auto msg = messages_.front();
				messages_.pop_front();
				memset(info.mText, 0, sizeof(info.mText));
				sprintf_s(info.mText, msg.c_str());
				info.mDestination = server_ ? 1 : 0;
				info.mTranslate = 0;
				return true;
			}
			return OnWantsToDisplayMessage(info);
		}

		void SetEnvironment(const EnvironmentInfoV01 &info)
		{
			OnSetEnvironment(info);
		}

		void InitScreen(const ScreenInfoV01 &info)
		{
			OnInitScreen(info);
		}

		void UninitScreen(const ScreenInfoV01 &info)
		{
			OnUninitScreen(info);
		}

		void DeactivateScreen(const ScreenInfoV01 &info)
		{
			OnDeactivateScreen(info);
		}

		void ReactivateScreen(const ScreenInfoV01 &info)
		{
			OnReactivateScreen(info);
		}

		void RenderScreenBeforeOverlays(const ScreenInfoV01 &info)
		{
			OnRenderScreenBeforeOverlays(info);
		}

		void RenderScreenAfterOverlays(const ScreenInfoV01 &info)
		{
			OnRenderScreenAfterOverlays(info);
		}

		void PreReset(const ScreenInfoV01 &info)
		{
			OnPreReset(info);
		}

		void PostReset(const ScreenInfoV01 &info)
		{
			OnPostReset(info);
		}

		bool InitCustomControl(CustomControlInfoV01 &info)
		{
			return OnInitCustomControl(info);
		}

		bool WantsWeatherAccess()
		{
			return OnWantsWeatherAccess();
		}

		bool AccessWeather(double trackNodeSize, WeatherControlInfoV01 &info)
		{
			return OnAccessWeather(trackNodeSize, info);
		}

		void ThreadStarted(long type)
		{
			OnThreadStarted(type);
		}

		void ThreadStopping(long type)
		{
			OnThreadStopping(type);
		}

		bool GetCustomVariable(long i, CustomVariableV01 &var)
		{
			return OnGetCustomVariable(i, var);
		}

		void AccessCustomVariable(CustomVariableV01 &var)
		{
			OnAccessCustomVariable(var);
		}

		void GetCustomVariableSetting(CustomVariableV01 &var, long i, CustomSettingV01 &setting)
		{
			OnGetCustomVariableSetting(var, i, setting);
		}

		bool WantsMultiSessionRulesAccess()
		{
			return OnWantsMultiSessionRulesAccess();
		}

		bool AccessMultiSessionRules(MultiSessionRulesV01 &info)
		{
			return OnAccessMultiSessionRules(info);
		}

		bool WantsTrackRulesAccess()
		{
			return OnWantsTrackRulesAccess();
		}

		bool AccessTrackRules(TrackRulesV01 &info)
		{
			return OnAccessTrackRules(info);
		}
		
		bool WantsPitMenuAccess()
		{
			return OnWantsPitMenuAccess();
		}

		bool AccessPitMenu(PitMenuV01 &info)
		{
			return OnAccessPitMenu(info);
		}
	};

} // namespace rf2
