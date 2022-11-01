# must do 'brew install --HEAD Gamma'
#
require 'formula'

class Gamma < Formula
  homepage 'https://github.com/mantaraya36/Gamma.git'
  head 'https://github.com/mantaraya36/Gamma.git', :tag => 'homebrew_testing'

  depends_on 'cmake' => :build
  depends_on 'libsndfile'
  depends_on 'portaudio'

  def install
    system "cmake", ".", *std_cmake_args
    system "make"
    system "make install"
  end
end
