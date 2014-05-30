#import <Syphon/Syphon.h>

SyphonServer *syphonServer = nil;

bool init_syphon_output()
{
  if (syphonServer)
  {
    NSLog(@"Try to init syphon server twice!");
    return false;
  }
  CGLContextObj context = CGLGetCurrentContext();
  syphonServer = [[SyphonServer alloc] initWithName:@"Robotron2084" context:context options:nil];
  return true;
}

void publish_texture(GLuint texture, GLenum target, GLuint width, GLuint height)
{
  [syphonServer publishFrameTexture:texture
                      textureTarget:target
                        imageRegion:NSMakeRect(0, 0, width, height)
                  textureDimensions:NSMakeSize(width, height)
                            flipped:YES];
}

void cleanup_syphon_output()
{
  [syphonServer stop];
  syphonServer = nil;
}