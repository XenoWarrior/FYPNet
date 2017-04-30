#define FYP_DEFAULT_PORT "7070"

// Socket status returns
enum FYPSocketStatus { FYP_SOCK_FAILURE, FYP_SOCK_SUCCESS };

// Client status returns
enum FYPClientStatus { FYP_ACCEPT_FAILIURE, FYP_ACCEPT_SUCCESS };

// Different response identifiers
enum FYPPacketType { FYP_OUT_WELCOME_RESPONSE, FYP_IN_CLIENT_MESSAGE, FYP_OUT_CLIENT_MESSAGE };