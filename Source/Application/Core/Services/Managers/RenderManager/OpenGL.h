#pragma once
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

namespace Nyx
{
	class GL
	{
	public:
		static void BindFrom(QOpenGLContext* ctx) 
		{
			s_funcs = ctx ? ctx->extraFunctions() : nullptr;
		}

		static QOpenGLExtraFunctions* Get()
		{
			return QOpenGLContext::currentContext()->extraFunctions();
		}

	private:
		static thread_local QOpenGLExtraFunctions* s_funcs;
	};
}