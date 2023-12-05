// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(ERROR_HH)
#    define ERROR_HH

#    include <nel/log.hh>
#    include <nel/panic.hh>

enum class ErrorCode {
    Failed = 1,
};

// std::ostream &operator<<(std::ostream &outs, ErrorCode const &val) {
nel::Log &operator<<(nel::Log &outs, ErrorCode const &val)
{
    // don't supply default case so compile fails if case not present for value in ErrorCode
    // but, must return in each case and must panic after switch.
    // see:
    // https://stackoverflow.com/questions/2201493/using-default-in-a-switch-statement-when-switching-over-an-enum
    switch (val) {
        case ErrorCode::Failed:
            return outs << "Failed";
            break;
        default:
            break;
    }
    nel::panic("unknown ErrorCode");
    return outs << '?';
}

// typedef uint16_t Lineno;
typedef long unsigned int Lineno;

struct Error
{
        // tot=16
    private:
        ErrorCode code; // offset:0, size:4 fsize:4
        Lineno lineno; // offset:4, size:4 fsize:2
        const char *filename; // offset:8, size:8 fsize:8

    public:
        constexpr Error(ErrorCode const &code, char const *filename, Lineno const line)
            : code(code)
            , lineno(line)
            , filename(filename)
        {
        }

    public:
        // friend std::ostream &operator<<(std::ostream &outs, Error const &val) {
        friend nel::Log &operator<<(nel::Log &outs, Error const &val)
        {
            outs << "Error(" << static_cast<int>(val.code) << "=" << val.code << ")";
            outs << "@" << val.filename << ":" << val.lineno;
            return outs;
        }
};

#endif // !defined(ERROR_HH)
