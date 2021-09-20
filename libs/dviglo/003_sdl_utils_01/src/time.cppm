export module dviglo.time;

// Модули движка
export import dviglo.base;
import <SDL.h>;

// Промежуток времени с прошлого кадра
export class DvTime
{
private:
    // Номер кадра.
    // Объекты могут хранить информацию, в каком кадре они что-то делали, чтобы не выполнять действия повторно в том же кадре
    u32 frame_number_ = 0;

    u32 old_ticks_ = 0;
    u32 time_step_ms_ = 0;
    float time_step_s_ = 0.f;
    
    u32 fps_frame_counter_ = 0;
    u32 fps_time_counter_ms_ = 0;
    i32 fps_ = -1;

public:
    inline u32 frame_number() const
    {
        return frame_number_;
    }

    inline u32 time_step_ms() const
    {
        return time_step_ms_;
    }

    inline float time_step_s() const
    {
        return time_step_s_;
    }
    
    inline i32 fps() const
    {
        return fps_;
    }

    void new_frame()
    {
        ++frame_number_;

        u32 new_ticks = SDL_GetTicks();
        time_step_ms_ = new_ticks - old_ticks_;
        old_ticks_ = new_ticks;
        time_step_s_ = time_step_ms_ / 1000.f;

        ++fps_frame_counter_;
        fps_time_counter_ms_ += time_step_ms_;

        // Обновляем ФПС каждые пол секунды
        if (fps_time_counter_ms_ >= 500)
        {
            fps_ = fps_frame_counter_ * 1000u / fps_time_counter_ms_;
            fps_frame_counter_ = 0;
            fps_time_counter_ms_ = 0;
        }
    }
};
