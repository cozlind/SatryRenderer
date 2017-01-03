#include "stdafx.h"
#include "FileIO.h"
#include "stdlib.h"
extern int numObj;
bool FileIO::OpenObjFile(const char path[],const char fileName[])
{
	char objPath[64], mtlPath[64];
	strcpy_s(filePath, path);
	strcpy_s(objPath, path);
	strcpy_s(mtlPath, path);
	strcat_s(objPath, fileName);
	errno_t err = fopen_s(&fp, objPath, "r");
	if (err == 0&&fp!=NULL)
	{
		printf("The file \"%s\" was opened\n", objPath);
	}
	else
	{
		printf("The file \"%s\" was not opened\n", objPath);
	}
	char buffer[100];
	while (!feof(fp) && fgets(buffer, 100, fp))
	{
		if (buffer[0] == 'm')
		{
			char mtlName[64];
			sscanf_s(buffer, "mtllib %s", mtlName, strlen(buffer));
			strcat_s(mtlPath, mtlName);
			err = fopen_s(&matFp, mtlPath, "r");
			if (err == 0 && matFp != NULL)
			{
				printf("The file \"%s\" was opened\n", mtlPath);
			}
			else
			{
				printf("The file \"%s\" was not opened\n", mtlPath);
			}
		}
		if (strcmp(buffer, "g default\n") == 0)
		{
			return true;
		}
	}
	return false;
}
void FileIO::Reset()
{
	numObj = 0;
	numV = 1;
	numUV = 1;
	numN = 1;
}
int FileIO::LoadAllMat(Material mats[])
{
	int numM = -1;
	char c1, c2, buffer[100];
	while (!feof(matFp) && fgets(buffer, 100, matFp))
	{
		char c_,c__,c___;
		sscanf_s(buffer, "%c%c%c", &c1, &c_, &c2);
		switch (c1)
		{
		case 'n':
			numM++;
			sscanf_s(buffer, "newmtl %s", mats[numM].name, strlen(buffer));
			break;
		case 'N':
			sscanf_s(buffer, "Ni %f", &mats[numM].ni);
			break;
		case 'i':
			sscanf_s(buffer, "illum %d", &mats[numM].attribute);//此处并没有和宏对应
			break;
		case 'K':
			switch (c2)
			{
			case 'd':
				sscanf_s(buffer, "Kd %f %f %f", &mats[numM].kd.x, &mats[numM].kd.y, &mats[numM].kd.z);
				break;
			case 'a':
				sscanf_s(buffer, "Ka %f %f %f", &mats[numM].ka.x, &mats[numM].ka.y, &mats[numM].ka.z);
				break;
			case 's':
				sscanf_s(buffer, "Ks %f %f %f", &mats[numM].ks.x, &mats[numM].ks.y, &mats[numM].ks.z);
				break;
			}
			break;
		case 'T':
			sscanf_s(buffer, "Tf %f %f %f", &mats[numM].tf.x, &mats[numM].tf.y, &mats[numM].tf.z);
			break;
		case 'm':
			char texName[64];
			sscanf_s(buffer, "map_K%c%c %s", &c2,&c___,&c__,&c_, texName, strlen(buffer));
			char texPath[64];
			strcpy_s(texPath, filePath);
			strcat_s(texPath, texName);
			readBitmap(texPath, mats[numM]);
		}
	}
	return numM+1;
}
void FileIO::readBitmap(char path[],Material& mat)
{
	FILE *bp;
	errno_t err = fopen_s(&bp, path, "rb");
	if (err == 0 && bp != NULL)
	{
		printf("The file \"%s\" was opened\n", path);
	}
	else
	{
		printf("The file \"%s\" was not opened\n", path);
	}
	//跳过位图文件头BITMAPFILEHEADER
	fseek(bp, sizeof(BITMAPFILEHEADER), 0);
	//读取位图信息头BITMAPINFOHEADER
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, bp);
	mat.texture.width = head.biWidth;
	mat.texture.height = head.biHeight;
	mat.texture.bitCount = head.biBitCount;
	mat.texture.lineBytes = (head.biWidth*head.biBitCount / 8 + 3) / 4 * 4;
	if (head.biBitCount != 8)
	{
		mat.texture.data = new unsigned char[head.biSizeImage];//head.biSizeImage==lineBytes*biHeight
		fread(mat.texture.data, 1, head.biSizeImage, bp);
		fclose(bp);
		return;
	}
	printf("The biBitCount of bitmap is 8\n");
	return;
}

void FileIO::readBitmap(char path[], Bmp& bmp)
{
	FILE *bp;
	errno_t err = fopen_s(&bp, path, "rb");
	if (err == 0 && bp != NULL)
	{
		printf("The file \"%s\" was opened\n", path);
	}
	else
	{
		printf("The file \"%s\" was not opened\n", path);
	}
	//跳过位图文件头BITMAPFILEHEADER
	fseek(bp, sizeof(BITMAPFILEHEADER), 0);
	//读取位图信息头BITMAPINFOHEADER
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, bp);
	bmp.width = head.biWidth;
	bmp.height = head.biHeight;
	bmp.bitCount = head.biBitCount;
	bmp.lineBytes = (head.biWidth*head.biBitCount / 8 + 3) / 4 * 4;
	if (head.biBitCount != 8)
	{
		bmp.data = new unsigned char[head.biSizeImage];//head.biSizeImage==lineBytes*biHeight
		fread(bmp.data, 1, head.biSizeImage, bp);
		fclose(bp);
		return;
	}
	printf("The biBitCount of bitmap is 8\n");
	return;
}

bool FileIO::LoadObjModel(Obj& obj, Material mats[])
{
	char c1,c2,buffer[100],mtlName[64]="";
	int numVT = 0, numVN = 0;
	float sumX = 0, sumY = 0, sumZ = 0;
	while (!feof(fp) && fgets(buffer, 100, fp))
	{
		char c_;
		sscanf_s(buffer, "%c%c%c", &c1,&c_,&c2);
		switch (c1)
		{
		case 'v':
			switch (c2)
			{
			case 't':
				sscanf_s(buffer, "vt %f%f", &obj.uvList[obj.numUV].x, &obj.uvList[obj.numUV].y);
				obj.numUV++;
				break;
			case 'n':
				sscanf_s(buffer, "vn %f%f%f", &obj.bakNormList[obj.numNorm].x, &obj.bakNormList[obj.numNorm].y, &obj.bakNormList[obj.numNorm].z);
				obj.numNorm++;
				break;
			default:
				sscanf_s(buffer, "v %f%f%f", &obj.bakVList[obj.numVertices].x, &obj.bakVList[obj.numVertices].y, &obj.bakVList[obj.numVertices].z);
				obj.bakVList[obj.numVertices].w = 1;
				sumX += obj.bakVList[obj.numVertices].x;
				sumY += obj.bakVList[obj.numVertices].y;
				sumZ += obj.bakVList[obj.numVertices].z;
				obj.numVertices++;
				break;
			}
			break;
		case 's'://平滑组
			char smoothGroup[10];
			sscanf_s(buffer, "s %s", smoothGroup,strlen(buffer));
			if (strcmp(smoothGroup, "off") == 0)
			{
				obj.attribute = SHADE_MODE_PURE;
			}
			else
			{
				obj.attribute = SHADE_MODE_GOURAUD;
			}
			break;
		case 'g':
			if (strcmp(buffer, "g default\n") == 0)
			{//一个模型读取结束
				numV += obj.numVertices;
				numN += obj.numNorm;
				numUV += obj.numUV;
				//通过平均来求得世界坐标和半径
				obj.worldPos.x = sumX / (float)obj.numVertices;
				obj.worldPos.y = sumY / (float)obj.numVertices;
				obj.worldPos.z = sumZ / (float)obj.numVertices;
				obj.worldPos.w = 1;
				float sumRadius=0, maxRadius=0;
				for (int i = 0; i < obj.numVertices; i++)
				{
					obj.bakVList[i].x -= obj.worldPos.x;
					obj.bakVList[i].y -= obj.worldPos.y;
					obj.bakVList[i].z -= obj.worldPos.z;
					obj.bakVList[i].w = 1;
					float radius= sqrt(obj.bakVList[i].x*obj.bakVList[i].x 
						+ obj.bakVList[i].y*obj.bakVList[i].y 
						+ obj.bakVList[i].z*obj.bakVList[i].z);
					sumRadius += radius;
					maxRadius = radius>maxRadius ? radius : maxRadius;
				}
				obj.avgRadius = sumRadius / (float)obj.numVertices;
				obj.maxRadius = maxRadius;
				return true;
			}
			sscanf_s(buffer, "g %s", obj.name, strlen(buffer));
			printf("Obj Name : %s\n", obj.name);
			break;
		case 'u':
			sscanf_s(buffer, "usemtl %s", mtlName, strlen(buffer));
			for (int i = 0; i < numMat; i++)
			{
				if (strcmp(mtlName, mats[i].name)==0)
				{
					obj.matIndex = i;
				}
			}
			break;
		case 'f':
		{
			int a[4], b[4],c[4],i=0;
			//按照空格进行分割
			const char* split = " ";
			char *bufferNode,*bufferRemain;
			bufferNode = strtok_s(buffer, split, &bufferRemain);
			bufferNode = strtok_s(NULL, split, &bufferRemain);
			while (bufferNode!=NULL)
			{
				int cj = 0, ck = 0,temp[3];
				char ch[10];
				for (int ci = 0; ci < strlen(bufferNode); ci++)
				{
					if (bufferNode[ci] != '/') {
						ch[cj++] = bufferNode[ci];
					}
					else {
						ch[cj] = '\0';
						if (cj == 0) temp[ck] = 0;
						else sscanf_s(ch, "%d", &temp[ck]);
						ck++;
						cj = 0;
					}
				}
				ch[cj] = '\0';
				if (cj == 0) temp[ck] = 0;
				else sscanf_s(ch, "%d", &temp[ck]);

				a[i] = temp[0];
				b[i] = temp[1];
				c[i] = temp[2];

				i++;
				bufferNode = strtok_s(NULL, split, &bufferRemain);
			}
			//sscanf_s(buffer, "f %d / %d / %d %d / %d / %d %d / %d / %d %d / %d / %d", &a[0], &b[0], &c[0], &a[1], &b[1], &c[1], &a[2], &b[2], &c[2], &a[3], &b[3], &c[3]);
			Poly& poly1 = obj.pList[obj.numPolys++];
			poly1.matIndex = obj.matIndex;
			poly1.vList = obj.vList;
			poly1.vert[0] = a[0] - numV;
			poly1.vert[1] = a[1] - numV;
			poly1.vert[2] = a[2] - numV;
			poly1.uvList = obj.uvList;
			poly1.uv[0] = b[0] - numUV;
			poly1.uv[1] = b[1] - numUV;
			poly1.uv[2] = b[2] - numUV;
			poly1.normList = obj.normList;
			poly1.norm[0] = c[0] - numN;
			poly1.norm[1] = c[1] - numN;
			poly1.norm[2] = c[2] - numN;
			poly1.reflectList = obj.reflectList;

			//存在一行三角面或四边面
			if (i > 3)
			{
				Poly& poly2 = obj.pList[obj.numPolys++];
				poly2.matIndex = obj.matIndex;
				poly2.vList = obj.vList;
				poly2.vert[0] = a[0] - numV;
				poly2.vert[1] = a[2] - numV;
				poly2.vert[2] = a[3] - numV;
				poly2.uvList = obj.uvList;
				poly2.uv[0] = b[0] - numUV;
				poly2.uv[1] = b[2] - numUV;
				poly2.uv[2] = b[3] - numUV;
				poly2.normList = obj.normList;
				poly2.norm[0] = c[0] - numN;
				poly2.norm[1] = c[2] - numN;
				poly2.norm[2] = c[3] - numN;
				poly2.reflectList = obj.reflectList;
			}
		}
		}
	}
	//最后一个模型读取结束
	numV = 1;
	numUV = 1;
	numN = 1;
	//通过平均来求得世界坐标和半径
	obj.worldPos.x = sumX / (float)obj.numVertices;
	obj.worldPos.y = sumY / (float)obj.numVertices;
	obj.worldPos.z = sumZ / (float)obj.numVertices;
	obj.worldPos.w = 1;
	float sumRadius = 0, maxRadius = 0;
	for (int i = 0; i < obj.numVertices; i++)
	{
		obj.bakVList[i].x -= obj.worldPos.x;
		obj.bakVList[i].y -= obj.worldPos.y;
		obj.bakVList[i].z -= obj.worldPos.z;
		obj.bakVList[i].w = 1;
		float radius = sqrt(obj.bakVList[i].x*obj.bakVList[i].x
			+ obj.bakVList[i].y*obj.bakVList[i].y
			+ obj.bakVList[i].z*obj.bakVList[i].z);
		sumRadius += radius;
		maxRadius = radius>maxRadius ? radius : maxRadius;
	}
	obj.avgRadius = sumRadius / (float)obj.numVertices;
	obj.maxRadius = maxRadius;
	return false;
}
void FileIO::CloseFile()
{
	fclose(fp);
}

FileIO::FileIO()
{
	fp = NULL;
}

FileIO::~FileIO()
{
}
