//
//  TextInputEventArgs.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.01.21.
//  Copyright � 2020 Matthias Gruen. All rights reserved.
//

#ifndef TextInputEventArgs_h
#define TextInputEventArgs_h

#include <Events/Event.h>

namespace Ice {

class TextInputEventArgs {
	friend class EventHandlingSystem;

	char m_char = '\0';
	bool m_bBackspace{ false };
	bool m_bEscape{ false };
	bool m_bEnter{ false };

public:
	TextInputEventArgs() = default;
	TextInputEventArgs(const TextInputEventArgs&) = default;
	TextInputEventArgs& operator=(const TextInputEventArgs&) = default;
	TextInputEventArgs(TextInputEventArgs&&) = default;
	TextInputEventArgs& operator=(TextInputEventArgs&&) = default;

	TextInputEventArgs(char ch) : m_char{ ch } {}
	auto character() const noexcept { return m_char; }

	bool isBackspace() const noexcept { return m_bBackspace; }
	bool isEscape() const noexcept { return m_bEscape; }
	bool isSpace() const noexcept { return false; }
	bool isEnter() const noexcept { return m_bEnter; }
};

}

#endif
