#include "request/impl/raw_request.hpp"

#include "response/impl/raw_response.hpp"

mt::network::RawRequest::RawRequest(Url p_url) :
    RequestBase(std::move(p_url)) { }

auto mt::network::RawRequest::requestData() -> const std::vector< std::byte >& { return m_request_data; }

void mt::network::RawRequest::initResponse() { m_response = std::make_shared< RawResponse >(m_id); }
