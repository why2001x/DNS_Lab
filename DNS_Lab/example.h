/**
 *  @brief     ��Ҫ˵��
 *  @details   ��ϸ˵�������ļ������ˣ�����
 *  @author    ��Ҫ����
 *  @author    ��������1 ��������2 ...
 *  @version   x.y.z
 *  @date      2020.07.xx-2020.xx.xx
 *  @pre       ǰ����������ʡ�ԣ�
 *  @bug       ���ǣ��ҵ��ϻ�ƣ�����funcʱ��Ƿ��ڴ���ʣ�����ָ����ˡ���û�о�ʡ�ԣ�
 *  @warning   ������ʹ�ÿ���������ϵͳ����ʡ�ԣ�
 *  @copyright GNU Public License.����ʡ�ԣ�
 */

#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif
    /**
	 * һ��ͨ���ṹ��
	 * ��ϸһ������ݣ���ϸ��ȥ����д
	 */
	typedef struct example
	{
		/// һ��һ�а�
		int a; ///< ˵��һ��
		int b; ///< ����������һ��
		       ///< һ��̫������������
	} example; ///< Ҳ��typedefһ�»���ã��Ҳ�ȷ��

	/// ���е���עд����
	extern int global; ///< ��������Ҳ���ԣ�ע��'<'��

	/**
	 * @brief ��Ҫ˵��
	 *
	 * ��Ȼд����ϸ�������
	 * 
	 * @param  a a�Ǹ�ɶ
	 * @param  b b���Ǹ�ɶ
	 * @return   ������ɶ��
	 */
	extern int func(int a, int b);

#ifdef __cplusplus
}
#endif

#endif // _EXAMPLE_H
