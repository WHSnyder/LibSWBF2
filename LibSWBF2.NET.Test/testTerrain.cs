using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using LibSWBF2.Logging;
using LibSWBF2.Wrappers;



namespace LibSWBF2.NET.Test
{
    class TerrainTest
    {
        public static int Main(string[] args)
        {   
            TestBench.StartLogging(ELogType.Warning);

            Level level = TestBench.LoadAndTrackLVL(args[0]);
            if (level == null) return -1;

            Terrain terrain = level.GetTerrains()[0];

            List<string> terrainLayerTextures = terrain.GetTextureNames();

            foreach (var str in terrainLayerTextures){
                
                if (str == ""){
                    continue;
                }

                Console.Write("Texture name: " + str);
                Texture tex = level.GetTexture(str);
                if (tex != null){
                    byte[] data = tex.GetBytesRGBA();
                    Console.WriteLine(" width: " + tex.width + " height: " + tex.height + " bytes length: " + data.Length);
                } else {
                    Console.WriteLine(" lookup failed.");
                }
            }
            
            terrain.GetHeightMap(out uint dim, out uint dimScale, out float[] heightMapData);
            terrain.GetBlendMap(out dim, out uint numLayers, out byte[] blendMapData);

            Console.WriteLine("Patches: ");
            for (int i = 0; i < (int) terrain.GetNumPatches(); i++)
            {
                Console.WriteLine("\tPatch {0}:", i);

                uint[] texSlots = terrain.GetPatchTextureIndices(i);
                byte[] blendMap = terrain.GetPatchBlendMap(i);
                float[] vbuf = terrain.GetPatchVertexBuffer(i);
                uint[] inds = terrain.GetPatchIndexBuffer(i);

                List<string> texesUsed = new List<string>();

                Console.WriteLine("\t\tTextures used:");
                for (int j = 0; j < texSlots.Length; j++)
                {
                    Console.WriteLine("\t\t\t" + terrainLayerTextures[(int) texSlots[j]]);
                }

                Console.WriteLine("\t\tBuffer lengths: Indices: {0}, Vertices: {1}, Blendmap: {2}", inds.Length, vbuf.Length / 3, blendMap.Length);
            }
            
            TestBench.StopLogging();
            return 1;
        }
    }
}
