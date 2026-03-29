FROM ubuntu:22.04

# Non-interactive
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    gcc \
    g++ \
    wget \
    unzip \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libglfw3-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxxf86vm-dev \
    libx11-dev \
    x11-xserver-utils \
    mesa-utils \
    && rm -rf /var/lib/apt/lists/*

# Install premake5
WORKDIR /tmp
RUN wget https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-linux.tar.gz \
    && tar -xzf premake-5.0.0-beta2-linux.tar.gz \
    && mv premake5 /usr/local/bin/ \
    && chmod +x /usr/local/bin/premake5 \
    && rm premake-5.0.0-beta2-linux.tar.gz

# GLFW selbst bauen (3.4+)
WORKDIR /tmp
RUN git clone https://github.com/glfw/glfw.git \
    && cd glfw \
    && git checkout 3.4 \
    && cmake -S . -B build \
        -DGLFW_BUILD_WAYLAND=OFF \
        -DGLFW_BUILD_X11=ON \
        -DGLFW_BUILD_DOCS=OFF \
        -DGLFW_BUILD_TESTS=OFF \
        -DGLFW_BUILD_EXAMPLES=OFF \
    && cmake --build build \
    && cmake --install build

# Arbeitsverzeichnis
WORKDIR /app

# Copy project
COPY . .

# Build
RUN rm -r build
RUN mkdir build
RUN premake5 gmake 
RUN cd build
RUN make config=release

# Default command
CMD ["./build/bin/Release/Softbody"]