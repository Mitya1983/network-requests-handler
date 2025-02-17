#ifndef HTTP_PARAM_HPP
#define HTTP_PARAM_HPP

#include "concepts.hpp"

#include <string>
#include <vector>
#include <optional>

namespace mt::network {

    struct Parameter {
        std::string name;
        std::string value;

        Parameter(std::string name_, std::string value_) :
            name(std::move(name_)),
            value(std::move(value_)) { }
        explicit Parameter(std::string_view::const_iterator& p_begin, std::string_view::const_iterator p_end);
    };

    class Params {
    public:
        Params() = default;
        explicit Params(std::string_view p_data);
        Params(const Params& p_other) = default;
        Params(Params&& p_other) noexcept = default;
        Params& operator=(const Params& p_other) = default;
        Params& operator=(Params&& p_other) noexcept = default;
        ~Params() = default;

        void addParameter(Parameter&& p_parameter);

        [[nodiscard]] auto parameterValue(std::string_view p_parameter_name) const -> std::optional< std::string_view >;
        [[nodiscard]] auto empty() const -> bool;
        [[nodiscard]] auto begin() noexcept -> std::vector< Parameter >::iterator;
        [[nodiscard]] auto begin() const noexcept -> std::vector< Parameter >::const_iterator;
        [[nodiscard]] auto cbegin() const noexcept -> std::vector< Parameter >::const_iterator;
        [[nodiscard]] auto end() noexcept -> std::vector< Parameter >::iterator;
        [[nodiscard]] auto end() const noexcept -> std::vector< Parameter >::const_iterator;
        [[nodiscard]] auto cend() const noexcept -> std::vector< Parameter >::const_iterator;

    private:
        std::vector< Parameter > m_params;
    };

}  // namespace mt::network::http

#endif  //HTTP_PARAM_HPP
