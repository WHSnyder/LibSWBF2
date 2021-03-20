using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


using LibSWBF2.Logging;
using LibSWBF2.Wrappers;
using LibSWBF2.Types;

namespace LibSWBF2.NET.Test
{
    class WorldInstancesRegionsTest
    {
        static int Main(string[] args)
        {
            TestBench testBench = new TestBench();

            Container container = testBench.LoadAndTrackContainer(new List<string>(args), out List<Level> lvls);

            Level level = lvls[0];
            if (level == null)
            {
                return -1;
            }
        
            World[] worlds = level.Get<World>();

            foreach (World world in worlds)
            {
                Console.WriteLine("\n" + world.Name);
                
                Console.WriteLine("  Instances: ");
                foreach (Instance instance in world.GetInstances())
                {
                    var ec = container.Get<EntityClass>(instance.EntityClassName);

                    if (ec == null)
                    {
                        continue;
                    }

                    string baseName = ec.BaseClassName;

                    
                    string instName = instance.Name;
                    Vector4 rot = instance.Rotation;
                    Vector3 pos = instance.Position;

                    Console.WriteLine("\n\tInstance: " + instName);
                    
                    Console.WriteLine("\t  Class: " + ec.Name);
                    Console.WriteLine("\t  Parent: " + baseName);
                    Console.WriteLine("\t  Rotation: " + rot.ToString());
                    Console.WriteLine("\t  Position: " + pos.ToString());

                    Console.WriteLine("\t  Overridden properties: ");
                    if (instance.GetOverriddenProperties(out uint[] props, out string[] values))
                    {
                        for (int j = 0; j < props.Length; j++)
                        {
                            Console.WriteLine("\t    Hash: {0}, Value: {1}", props[j], values[j]);
                        }
                    }                  
                }


                Console.WriteLine("  Regions:");
                foreach (Region region in world.GetRegions())
                {
                    Console.WriteLine("\n\tRegion: " + region.Name);
                
                    Console.WriteLine("\t  Size: " + region.Size.ToString());
                    Console.WriteLine("\t  Rotation: " + region.Rotation.ToString());
                    Console.WriteLine("\t  Type: " + region.Type);
                } 
            }

            return 1;
        }
    }
}
