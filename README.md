# A TOY HTTP SERVER

This is my toy implementation of an extremely basic http server.

## High Level Design Targets

1. **Core Server Layer**
   - [X] **Main Listener:** Accepts TCP connections.
   - [x] **Concurrency:** Start with a simple thread-per-connection or thread pool; upgrade to async/event-driven I/O (later).
   - [ ] **SSL/TLS:** Add support for HTTPS (later).

2. **Request Processing Pipeline**
   - [x] **Parser:** Parse incoming HTTP requests into struct, upgrade to a hash map. (later)
   - [x] **Router:** Simple URL mapping to resolve static file paths.
   - [x] **File I/O:** Read files from disk with MIME type detection.
   - [x] **Encoder:** Build and encode HTTP responses.

3. **Cross-Cutting Concerns (Low-Hanging Fruit)**
   - [x] **Logging & Monitoring:** Log requests, errors, and performance metrics.
   - [x] **Error Handling:** Return proper 404/500 responses.
   - [x] **Security:** Prevent path traversal and implement basic access control.
   - [ ] **Compression:** Support gzip/Brotli to reduce payload sizes.
   - [ ] **Caching:** In-memory LRU cache for frequently accessed files.
   - [x] **Directory Indexing:** Support for index files or autoindex as needed.

4. **Future Enhancements**
   - [ ] **Rate Limiting:** Mitigate abuse and DoS attacks.
   - [ ] **CDN Integration:** Offload and globally cache static assets.

## References

- [Man Pages](https://linux.die.net/man/)
- [Beej's Guide](https://beej.us/guide/bgnet/)
- [The Makefile Tutorial](https://makefiletutorial.com/ )
- [The Linux Programming Interface](https://nostarch.com/tlpi/)
