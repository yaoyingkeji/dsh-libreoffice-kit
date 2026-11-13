// Linux stub for the frame-forwarding SDK. The real transport and producer
// sources target macOS (IOSurface/Mach) and Windows (D3D11/shared handles);
// neither backend is available on Linux, so this stub provides link-time
// definitions that let the native engine build. Every connect attempt fails,
// which RenderConnection turns into a runtime exception, so frame forwarding
// is simply unavailable on Linux rather than silently broken.
#include "dsh_frame_forwarding.h"
#include <cstdio>

extern "C" {

dsh_frame_forwarding_client* dsh_frame_forwarding_connect(const char*, char* error, size_t error_size) {
    if (error && error_size) std::snprintf(error, error_size, "frame-forwarding unavailable on Linux");
    return nullptr;
}

dsh_frame_forwarding_client* dsh_frame_forwarding_connect_with_device(const char*, void*, char* error, size_t error_size) {
    if (error && error_size) std::snprintf(error, error_size, "frame-forwarding unavailable on Linux");
    return nullptr;
}

dsh_frame_forwarding_client* dsh_frame_forwarding_connect_with_options(const char*, const dsh_frame_forwarding_connect_options*, char* error, size_t error_size) {
    if (error && error_size) std::snprintf(error, error_size, "frame-forwarding unavailable on Linux");
    return nullptr;
}

dsh_frame_forwarding_surface* dsh_frame_forwarding_acquire(dsh_frame_forwarding_client*, uint32_t, uint32_t) { return nullptr; }
void* dsh_frame_forwarding_surface_native(dsh_frame_forwarding_surface*) { return nullptr; }
void* dsh_frame_forwarding_surface_shared_handle(dsh_frame_forwarding_surface*) { return nullptr; }
void* dsh_frame_forwarding_device(dsh_frame_forwarding_client*) { return nullptr; }
void* dsh_frame_forwarding_surface_keyed_mutex(dsh_frame_forwarding_surface*) { return nullptr; }
int dsh_frame_forwarding_write_bgra(dsh_frame_forwarding_surface*, const void*, size_t) { return 0; }
int dsh_frame_forwarding_write_bgra_with_timeout(dsh_frame_forwarding_surface*, const void*, size_t, uint32_t) { return 0; }
int dsh_frame_forwarding_submit(dsh_frame_forwarding_client*, dsh_frame_forwarding_surface*, uint64_t, const char*) { return 0; }
int dsh_frame_forwarding_submit_latest(dsh_frame_forwarding_client*, dsh_frame_forwarding_surface*, uint64_t, const char*) { return 0; }
int dsh_frame_forwarding_trim(dsh_frame_forwarding_client*) { return 0; }
void dsh_frame_forwarding_discard(dsh_frame_forwarding_client*, dsh_frame_forwarding_surface*) {}
int dsh_frame_forwarding_poll(dsh_frame_forwarding_client*, uint32_t, dsh_frame_forwarding_event*) { return -1; }
int dsh_frame_forwarding_send(dsh_frame_forwarding_client*, const char*) { return 0; }
void dsh_frame_forwarding_interrupt(dsh_frame_forwarding_client*) {}
void dsh_frame_forwarding_close(dsh_frame_forwarding_client*) {}

}
