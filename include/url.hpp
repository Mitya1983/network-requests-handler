#ifndef URL_HPP
#define URL_HPP

#include "ipv4.hpp"
#include <string>
#include <system_error>
#include <vector>


namespace mt::network {
    class Url {

    public:
        Url() noexcept = default;
        explicit Url(const std::string& p_url);
        Url(const Url& p_other) = default;
        Url(Url&& p_other) noexcept = default;
        Url& operator=(const Url& p_other) = default;
        Url& operator=(Url&& p_other) noexcept = default;
        ~Url() = default;

        void setScheme(std::string p_scheme);
        void setAuthority(std::string p_host, std::string p_user_name = "", std::string p_user_password = "");
        void addHostIP(std::string p_ip);
        void setPort(uint16_t p_port, std::endian p_endian = std::endian::big);
        void setPort(const std::string& p_port);
        void setPath(std::string p_path);
        void setQuery(std::string p_query);
        void setFragment(std::string p_fragment);

        [[nodiscard]] auto scheme() const noexcept -> const std::string&;
        [[nodiscard]] auto userName() const noexcept -> const std::string&;
        [[nodiscard]] auto userPassword() const noexcept -> const std::string&;
        [[nodiscard]] auto host() const noexcept -> const std::string&;
        [[nodiscard]] auto hostIP() const noexcept -> Ipv4;
        [[nodiscard]] auto hostIPList() const noexcept -> const std::vector< Ipv4 >&;
        [[nodiscard]] auto port() const noexcept -> std::string;
        [[nodiscard]] auto port_local_byte_order() const noexcept -> uint16_t;
        [[nodiscard]] auto port_network_byte_order() const noexcept -> uint16_t;
        [[nodiscard]] auto path() const noexcept -> const std::string&;
        [[nodiscard]] auto query() const noexcept -> const std::string&;
        [[nodiscard]] auto fragment() const noexcept -> const std::string&;
        [[nodiscard]] auto composeUrl() const -> std::string;
        [[nodiscard]] auto valid() const noexcept -> bool;
        [[nodiscard]] auto resolved() const noexcept -> bool;
        [[nodiscard]] auto error() const noexcept -> std::error_code;

        void resolve();
    private:
        std::string m_scheme;
        std::string m_user_name;
        std::string m_user_password;
        std::string m_host;
        std::string m_path;
        std::string m_query;
        std::string m_fragment;

        std::vector< Ipv4 > m_host_ip{};

        std::error_code m_error;

        uint16_t m_port{0};

        bool m_valid{false};
        bool m_resolved{false};
    };

}  // namespace tristan::network
#endif  //URL_HPP
