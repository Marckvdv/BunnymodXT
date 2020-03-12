#pragma once

namespace CustomTriggers
{
	// To be extended as new ways of defining the triggers are added.
	class Trigger
	{
		// Opposite corners of an axis-aligned parallelepiped.
		Vector corner_min, corner_max;

		// The command to be executed.
		std::string command;

		// Whether the player is currently touching this trigger.
		bool player_touching;

		// Normalize the corners so corner_min has the min coordinates and corner_max the max ones.
		void normalize();

		void touch();

	public:
		Trigger(Vector corner1, Vector corner2);
		Trigger(Vector corner1, Vector corner2, std::string command);

		std::pair<Vector, Vector> get_corner_positions() const;
		const std::string& get_command() const;
		void set_command(std::string new_command);
		void set_corner_positions(Vector a, Vector b);

		void update(const Vector& player_position, bool ducking);

		/*
		 * The player has moved from player_position_start to player_position_end.
		 * Check if they touched the trigger anywhere during that (linear) motion.
		 */
		void update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
	};

	class CustomTriggers
	{
		std::vector<Trigger> triggers;
		bool placing = false;
		Vector place_start;

		typedef Trigger* iterator;
		typedef const Trigger* const_iterator;

	public:
		iterator begin() { return &triggers[0]; }
		const_iterator begin() const { return &triggers[0]; }
		iterator end() { return &triggers[triggers.size()]; }
		const_iterator end() const { return &triggers[triggers.size()]; }

		void pop();
		bool empty();
		void clear();
		size_t size();

		Trigger& last_trigger();
		Trigger& get_trigger(size_t id);
		void new_trigger(Vector corner1, Vector corner2);
		void remove_trigger(size_t id);
		void start_placing(Vector start);\
		void stop_placing();
		bool is_placing();
	};

	extern CustomTriggers triggers;

	void Update(const Vector& player_position, bool ducking);
	void Update(const Vector& player_position_start, const Vector& player_position_end, bool ducking);
}
